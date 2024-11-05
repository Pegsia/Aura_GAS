// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Teleport.h"

bool UAuraGameplayAbility_Teleport::TeleportWithNoCheck(FVector DestLocation, FRotator DestRotation)
{
	return GetAvatarActorFromActorInfo()->TeleportTo(DestLocation, DestRotation, false, true);
}

FString UAuraGameplayAbility_Teleport::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
		
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Teleport</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Teleport To Cursor Location </>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_Teleport::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));

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
			"<Default>Teleport To Cursor Location </>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown);
	return NextLevelString;
}

