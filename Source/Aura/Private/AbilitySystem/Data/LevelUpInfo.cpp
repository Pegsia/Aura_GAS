// Copyright QiuXuan


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLeveForXP(int32 XP)
{
	int32 Level = 0;
	// LevelUpInformation[0] = {0, 0, 0}; Index[0] Is a placeholder
	// Example: Level[1] require 300XP, when we just have 300XP, we consider to LevelUp to Level2 and remaining 0XP
	for(const FAuraLevelUpInfo& Info : LevelUpInformation)
	{
		if(XP < Info.LevelUpRequirement) return Level;
		++Level;
	}
	return Level;
}

float ULevelUpInfo::FindPercentForXP(const int32 XP)
{
	const int32 Level = FindLeveForXP(XP);
	if(Level > LevelUpInformation.Num() || Level == 0) return 0.f; // Never to true, just ensure
	// LevelUpInformation[0] is {0,0,0}, Level is Grater than 0
	const int32 RemainingXP = XP - LevelUpInformation[Level - 1].LevelUpRequirement; 
	const int32 RequirementXP = LevelUpInformation[Level].LevelUpRequirement - LevelUpInformation[Level - 1].LevelUpRequirement;
	return RemainingXP * 1.f / RequirementXP;
}
