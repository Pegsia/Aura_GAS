// Copyright QiuXuan


#include "Character/AuraCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "AuraHUD.h"
#include "AuraPlayerController.h"
#include "MotionWarpingComponent.h"
#include "Aura/Aura.h"
#include "LevelUpInfo.h"
#include "NiagaraComponent.h"

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
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitialAbilityActorInfo();
	InitAuraStartupAbilities(); // Called Before Widget Controller Set
	// Init Attribute through GameplayEffects in AuraCharacterBase
	InitialDefaultAttributes(); // 可以只在Server端调用，因为所有变量都是Replicated的，而且ASC复制模式为Mixed	
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
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
	
	AuraPlayerState->OnLevelChangeDelegate.AddLambda(
		[this](int32 Value)
		{
			MulticastLevelUpVFX();
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

void AAuraCharacter::AddToXP_Implementation(int32 InXP) // AuraAS
{
	GetAuraPSChecked()->AddToXP(InXP);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation() // ExecCalc, MMC
{
	return GetAuraPSChecked()->GetPlayerLevel();
}

void AAuraCharacter::MulticastLevelUpVFX_Implementation() const
{
	if(IsValid(LevelUpNiagaraComponent))
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

int32 AAuraCharacter::GetXP_Implementation() const
{
	return GetAuraPSChecked()->GetXP();
}

AAuraPlayerState* AAuraCharacter::GetAuraPSChecked() const
{
	return GetPlayerStateChecked<AAuraPlayerState>();
}
