// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_ArcaneShards.h"

FString UAuraGameplayAbility_ArcaneShards::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 ArcaneShardAmount = FMath::Min(AbilityLevel, MaxArcaneShardsSpawnNumber);
	const FString ArcaneString = ArcaneShardAmount == 1 ? FString(TEXT("a Arcane Shard")) : FString::Printf(TEXT("%d Arcane Shards"), ArcaneShardAmount);
	
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Arcane Shards</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Summon %s, causing </>"
			//Damage
			"<Damage>%d </><Default>radial arcane damage at the shard origins.</>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown,
			 *ArcaneString,
			 Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_ArcaneShards::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 PreDamage = DamageAmount.GetValueAtLevel(AbilityLevel - 1);
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 ArcaneShardAmount = FMath::Min(AbilityLevel, MaxArcaneShardsSpawnNumber);
	const FString ArcaneString = ArcaneShardAmount == 1 ? FString(TEXT("a Arcane Shard")) : FString::Printf(TEXT("%d Arcane Shards"), ArcaneShardAmount);
	
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
			"<Default>Summon %s, causing </>"
			//Damage
			"<Gray>%d -> </><Damage>%d </><Default>radial arcane damage at the shard origins.</>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown,
			*ArcaneString,
			PreDamage, Damage);
	return NextLevelString;
}
