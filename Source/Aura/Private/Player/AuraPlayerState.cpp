// Copyright QiuXuan


#include "Player/AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;

	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	if(InXP < 0) return;
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	if(InLevel < 0) return;
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
}
