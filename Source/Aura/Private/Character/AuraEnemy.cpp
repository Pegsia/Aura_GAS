// Copyright QiuXuan


#include "Character/AuraEnemy.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraAIController.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "AuraUserWidget.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// For CursorTrace() in AuraPlayerController 
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBarComponent->SetupAttachment(GetRootComponent());

	// Movement
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	// Add Actor Tag for BTS_FindNearestPlayer
	Tags.Emplace(ACTOR_TAG_ENEMY);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	BaseWalkSpeed = 250.f;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;
	AuraAIController = Cast<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(HitReactingKey, false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(RangedAttackerKey, CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// For OutLineColor
	GetMesh()->SetCustomDepthStencilValue(HighLightColor.GetIntValue()); // 放在构造函数中只能使用默认值，因为在蓝图中更改需要程序运行阶段的反射起作用
	Weapon->SetCustomDepthStencilValue(HighLightColor.GetIntValue());
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	InitialAbilityActorInfo();		// Init ASC
	InitEffects();					// Init Hit Effect
	if(HasAuthority())				// Both Need GameMode which only exist on the Server
	{
		InitialDefaultAttributes(); // Init Default Attributes in AuraCharacterBase
		UAuraAbilitySystemLibrary::InitializeStartAbilities(this, AbilitySystemComponent, CharacterClass);
	}	
	InitHealthBar();		        // Init Attributes CallBacks
}

void AAuraEnemy::InitialAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::InitEffects()
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	AbilitySystemComponent->RegisterGameplayTagEvent(AuraTags.Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraEnemy::HitReactTagChanged
	);

	AbilitySystemComponent->RegisterGameplayTagEvent(AuraTags.Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraEnemy::StunTagChanged
	);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallBackTag, int32 TagCount)
{
	bHitReacting = TagCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if(!HasAuthority()) return;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(HitReactingKey, bHitReacting);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 TagCount)
{
	Super::StunTagChanged(CallbackTag, TagCount);
	if(!HasAuthority()) return;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(StunnedKey, bIsStunned);
}

void AAuraEnemy::InitHealthBar()
{
	if (UAuraUserWidget* HealthBar = Cast<UAuraUserWidget>(HealthBarComponent->GetUserWidgetObject()))
	{
		HealthBar->SetWidgetController(this);
	}

	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
		);

		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::HighLightActor_Implementation()
{
	// PostProcessVolume- Post Process Materials
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighLightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutLocation)
{
	// Don't Implement this, we do nothing for Enemies 
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraEnemy::CharacterDeath(const FVector& ImpulseVector)
{
	HealthBarComponent->DestroyComponent();
	AuraAIController->GetBrainComponent()->StopLogic(TEXT("Died"));
	SetLifeSpan(EnemyLifeSpan);
	Super::CharacterDeath(ImpulseVector);
}
