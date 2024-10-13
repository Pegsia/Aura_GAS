// Copyright QiuXuan


#include "Player/AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "LevelUpInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;

	// Init ASC and AS
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	if(InXP < 0)
	{
		UE_LOG(LogAura, Error, TEXT("Trying to Assign an InValid XP: %d"), InXP);
		return;
	}
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	if(InLevel < 0)
	{
		UE_LOG(LogAura, Error, TEXT("Trying to Assign an InValid Level: %d"), InLevel);
		return;
	}
	Level = InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetAttributePoints(int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	bool bLeveledUp = false;
	while(LevelUpInfo->CanLevelUp(Level, XP))
	{
		bLeveledUp = true;
		const int32 AttributePointsToReward = LevelUpInfo->LevelUpInformation[Level].AttributePointReward;
		const int32 SpellPointsToReward = LevelUpInfo->LevelUpInformation[Level].SpellPointReward;
		AddToAttributePoints(AttributePointsToReward);
		AddToSpellPoints(SpellPointsToReward);		
		AddToLevel(1);		
	}
	if(bLeveledUp) // Don't do this in while loop, when leveling up multiply times ABILITYLIST_SCOPE_LOCK will prevent as adding abilities
	{
		GetAuraAS()->RefillVitalAttributes();
		GetAuraASC()->UpdateAbilityStatuses(Level);
	}	
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributesPoints)
{
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAuraAbilitySystemComponent* AAuraPlayerState::GetAuraASC() const
{
	return CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}

UAuraAttributeSet* AAuraPlayerState::GetAuraAS() const
{
	return CastChecked<UAuraAttributeSet>(AttributeSet);
}
