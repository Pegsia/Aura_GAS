// Copyright QiuXuan


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "AuraUserWidget.h"
#include "AuraWidgetController.h"
#include "AuraAbilitySystemLibrary.h"

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
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitialAbilityActorInfo(); // Init ASC
	InitialDefaultAttributes();// Init Default Attributes in AuraCharacterBase
	InitHealthBar();		   // Init Attributes CallBacks
}

void AAuraEnemy::InitialAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();	
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