// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_FireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraGameplayAbility_FireBolt::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 Damage = GetDamageByDamageType(AbilityLevel, FAuraGameplayTags::Get().Damage_Fire);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, NumProjectiles);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Fire Bolt</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// ManaCost
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			//Damage
			"<Damage>%d </><Default>fire damage with a chance to burn.</>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown,
			 *FireBoltString,
			 Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_FireBolt::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 PreDamage = GetDamageByDamageType(AbilityLevel - 1, FAuraGameplayTags::Get().Damage_Fire);
	const int32 Damage = GetDamageByDamageType(AbilityLevel, FAuraGameplayTags::Get().Damage_Fire);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, NumProjectiles);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
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
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			//Damage
			"<Gray>%d -> </><Damage>%d </><Default>fire damage with a chance to burn.</>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown,
			*FireBoltString,
			PreDamage, Damage);
	return NextLevelString;
}
