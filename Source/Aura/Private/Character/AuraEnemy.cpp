// Copyright QiuXuan


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
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
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	InitialAbilityActorInfo();		// Init ASC
	InitEffects();					// Init Hit Effect
	if(HasAuthority())				// Both Need GameMode which only exist on the Server
	{
		InitialDefaultAttributes(); // Init Default Attributes in AuraCharacterBase
		UAuraAbilitySystemLibrary::InitializeStartAbilities(this, AbilitySystemComponent);
	}	
	InitHealthBar();		        // Init Attributes CallBacks
}

void AAuraEnemy::InitialAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();	
}

void AAuraEnemy::InitEffects()
{
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraEnemy::HitReactTagChanged
	);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallBackTag, int32 TagCount)
{
	bHitReacting = TagCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(HitReactingKey, bHitReacting);
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

void AAuraEnemy::HighLightActor()
{
	// PostProcessVolume- Post Process Materials
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighLightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::CharacterDeath()
{
	SetLifeSpan(EnemyLifeSpan);
	Super::CharacterDeath();
}
