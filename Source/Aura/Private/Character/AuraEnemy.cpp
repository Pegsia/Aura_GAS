// Copyright QiuXuan


#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"

AAuraEnemy::AAuraEnemy()
{
	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// For CursorTrace() in AuraPlayerController 
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitialAbilityActorInfo();
}

void AAuraEnemy::InitialAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
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



