// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

FString UAuraGameplayAbility::GetCurrentLevelDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), TEXT("Default Ability Name - TioPlato"), AbilityLevel);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</>\n<Default>Causes much more damage</>"), AbilityLevel);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>Ability Locked Until Level: %d</>"), AbilityLevel);
}
