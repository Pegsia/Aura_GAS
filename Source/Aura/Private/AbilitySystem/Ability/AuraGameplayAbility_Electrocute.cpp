// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Electrocute.h"

FString UAuraGameplayAbility_Electrocute::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 AdditionalTargetAmount = FMath::Min(AbilityLevel - 1, AdditionalTargetNumber);
	const FString BeamString = AdditionalTargetAmount == 0 ? FString(TEXT("")) : FString::Printf(TEXT("propagating to %d additional target nearby, "), AdditionalTargetAmount);
	
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Electrocute</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Emits a beam of lightning, connecting with the target, %srepertedly causing: </>"
			//Damage
			"<Damage>%d </><Default>lightning damage with a chance to Stun.</>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown,
			 *BeamString,
			 Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_Electrocute::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 PreDamage = DamageAmount.GetValueAtLevel(AbilityLevel - 1);
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 AdditionalTargetAmount = FMath::Min(AbilityLevel - 1, AdditionalTargetNumber);
	const FString BeamString = AdditionalTargetAmount == 0 ? FString(TEXT("")) : FString::Printf(TEXT("propagating to %d additional target nearby, "), AdditionalTargetAmount);
	
	const FString NextLevelString = FString::Printf(TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"
			// AbilityLevel
			"<Small>Level: </><Gray>%d -> </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><Gray>%.1f -> </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Gray>%.1f -> </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Emits a beam of lightning, connecting with the target, %srepertedly causing: </>"
			//Damage
			"<Gray>%d -> </><Damage>%d </><Default>lightning damage with a chance to Stun.</>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown,
			*BeamString,
			PreDamage, Damage);
	return NextLevelString;
}
