// Copyright QiuXuan


#include "Character/AuraCharacter.h"

#include "AbilityInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraAttributeSet.h"
#include "AuraGameModeBase.h"
#include "AuraGameplayTags.h"
#include "AuraHUD.h"
#include "AuraPlayerController.h"
#include "AuraSaveGame_LoadSlot.h"
#include "MotionWarpingComponent.h"
#include "Aura/Aura.h"
#include "NiagaraComponent.h"
#include "Debuff/AuraDebuffNiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Passive/AuraPassiveNiagaraComponent.h"

AAuraCharacter::AAuraCharacter()
{
	// Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	// TopDown Movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // 向加速度方向旋转
	GetCharacterMovement()->RotationRate = AuraRotationRate;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;	

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	CharacterClass = ECharacterClass::HeroAura;
	Tags.Emplace(ACTOR_TAG_PLAYER);
	
	//  Passive Niagara Component
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetUsingAbsoluteRotation(true);   // Don't Rotate with Root
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
		
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UAuraPassiveNiagaraComponent>("HaloOfProtectionComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UAuraPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UAuraPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitialAbilityActorInfo();
	LoadProgress();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the Client
	InitialAbilityActorInfo();
}

void AAuraCharacter::InitialAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	// Init ASC, AS
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraCharacter::StunTagChanged
	);
	
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
	
	AuraPlayerState->OnLevelChangeDelegate.AddLambda(
		[this](int32 Value, bool bLoading)
		{
			MulticastLevelUpVFX(bLoading);
		});
	
	/**
	 * Init AuraHUD
	 *  - Set UAuraWideget(WBP_Overlay) && UAuraWidgetController(BP_OverlayWidgetController)
	 *  - Bind Call Backs through WidgetController
	 *	- Broadcast Init Attributes
	 */ 
	if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
	{
		if (AAuraHUD* AuraHUD = AuraPlayerController->GetHUD<AAuraHUD>())
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}	
}

void AAuraCharacter::LoadProgress()
{
	if(AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if(UAuraSaveGame_LoadSlot* LoadSlot = AuraGameModeBase->LoadInGameProgressData())
		{
			if(LoadSlot->bInitializingSaveGame) //Create a save game,use default
			{
				InitAuraStartupAbilities(); // Called Before Widget Controller Set
				InitialDefaultAttributes(); // Init Attribute through GameplayEffects in AuraCharacterBase, 可以只在Server端调用，因为所有变量都是Replicated的，而且ASC复制模式为Mixed 
			}
			else
			{
				GetAuraASCChecked()->AddCharacterLoadedAbilities(LoadSlot);
				InitialStateFromSaveGame(LoadSlot);
				AuraGameModeBase->LoadWorldState(GetWorld()); // Load Actors
			}			
		}
	}
}

void AAuraCharacter::InitialStateFromSaveGame(const UAuraSaveGame_LoadSlot* LoadSlot)
{
	AAuraPlayerState* AuraPlayerState = GetAuraPSChecked();
	AuraPlayerState->SetLevel(LoadSlot->PlayerLevel);
	AuraPlayerState->SetXP(LoadSlot->PlayerXP);
	AuraPlayerState->SetAttributePoints(LoadSlot->AttributePoints);
	AuraPlayerState->SetSpellPoints(LoadSlot->SpellPoints);

	UAuraAbilitySystemLibrary::InitializeCharacterDefaultAttributesFromSaveData(this, AbilitySystemComponent, LoadSlot);
}

void AAuraCharacter::SaveProgress_Implementation(const FName& PlayerStartTag)
{
	/** Server Only */
	if(AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if(UAuraSaveGame_LoadSlot* LoadSlot = AuraGameModeBase->LoadInGameProgressData())
		{
			LoadSlot->PlayerStartTag = PlayerStartTag;

			/** Player */
			AAuraPlayerState* AuraPlayerState = GetAuraPSChecked();
			LoadSlot->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			LoadSlot->PlayerXP = AuraPlayerState->GetXP();
			LoadSlot->AttributePoints = AuraPlayerState->GetAttributePoints();
			LoadSlot->SpellPoints = AuraPlayerState->GetSpellPoints();

			// Last Transform Player Start
			LoadSlot->PlayerLastTransform = GetActorTransform();
				
			/** Attributes */
			UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
			LoadSlot->Strength = AuraAttributeSet->GetStrength();
			LoadSlot->Intelligence = AuraAttributeSet->GetIntelligence();
			LoadSlot->Resilience = AuraAttributeSet->GetResilience();
			LoadSlot->Vigor = AuraAttributeSet->GetVigor();

			LoadSlot->bInitializingSaveGame = false;

			/** Abilities */
			UAuraAbilitySystemComponent* AuraASC = GetAuraASCChecked();
			UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FForEachAbilitySignature ForEachAbilityDelegate;
			LoadSlot->SaveGameAbilities.Empty();
			ForEachAbilityDelegate.BindLambda([this, Info, LoadSlot](const FGameplayAbilitySpec& Spec)
			{
				const FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(Spec);
				const FGameplayTag AbilityInputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(Spec);
				const FGameplayTag AbilityStatusTag = UAuraAbilitySystemComponent::GetAbilityStatusTagFromSpec(Spec);
				
				FAuraAbilityInfo AbilityInfo = Info->FindAbilityInfoByAbilityTag(AbilityTag);
				
				FSaveGameAbility SaveGameAbility;
				SaveGameAbility.AbilityClass = AbilityInfo.AbilityClass;
				SaveGameAbility.AbilityTag = AbilityTag;
				SaveGameAbility.AbilityInputTag = AbilityInputTag;
				SaveGameAbility.AbilityStatusTag = AbilityStatusTag;
				SaveGameAbility.AbilityTypeTag = AbilityInfo.AbilityTypeTag;
				SaveGameAbility.AbilityLevel = Spec.Level;

				LoadSlot->SaveGameAbilities.AddUnique(SaveGameAbility);
			});
			AuraASC->ForEachAbility(ForEachAbilityDelegate);
			
			AuraGameModeBase->SaveInGameProgressData(LoadSlot);
		}		
	}
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP) // AuraAS
{
	GetAuraPSChecked()->AddToXP(InXP);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation() // ExecCalc, MMC
{
	return GetAuraPSChecked()->GetPlayerLevel();
}

void AAuraCharacter::CharacterDeath(const FVector& ImpulseVector)
{
	Super::CharacterDeath(ImpulseVector);

	if(AAuraGameModeBase* GameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		FTimerDelegate DeathTimerDelegate;
		DeathTimerDelegate.BindLambda([this, GameModeBase]()
		{
			GameModeBase->PlayerDead(this);
		});
		GetWorldTimerManager().SetTimer(DeathTimerHandle, DeathTimerDelegate, DeathWaitTime, false);
		CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // 死亡之后摄像机会掉下去
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if(bIsBurned)
	{
		BurnDebuffNiagaraComponent->Activate();
	}
	else
	{
		BurnDebuffNiagaraComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(AuraTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(AuraTags.Player_Block_InputHeld);
		BlockedTags.AddTag(AuraTags.Player_Block_InputReleased);
		BlockedTags.AddTag(AuraTags.Player_Block_InputPressed);
		if(bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Deactivate();
		}
	}
}

void AAuraCharacter::MulticastLevelUpVFX_Implementation(bool bLoading) const
{
	if(IsValid(LevelUpNiagaraComponent) && !bLoading)
	{
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 AAuraCharacter::GetAttributePoint_Implementation()
{
	return GetAuraPSChecked()->GetAttributePoints();
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	GetAuraPSChecked()->AddToAttributePoints(InAttributePoints);
}

int32 AAuraCharacter::GetSpellPoint_Implementation()
{
	return GetAuraPSChecked()->GetSpellPoints();
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	GetAuraPSChecked()->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	GetAuraPCChecked()->ShowMagicCircle(DecalMaterial);
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	GetAuraPCChecked()->HideMagicCircle();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	return GetAuraPSChecked()->GetXP();
}

AAuraPlayerState* AAuraCharacter::GetAuraPSChecked() const
{
	return GetPlayerStateChecked<AAuraPlayerState>();
}

AAuraPlayerController* AAuraCharacter::GetAuraPCChecked() const
{
	return CastChecked<AAuraPlayerController>(GetController());
}

UAuraAbilitySystemComponent* AAuraCharacter::GetAuraASCChecked() const
{
	return CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
}
