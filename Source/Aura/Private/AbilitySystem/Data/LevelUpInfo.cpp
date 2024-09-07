// Copyright QiuXuan


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLeveForXP(int32 XP)
{
	int32 Level = 0;
	// LevelUpInformation[0] = {-1, 0, 0}; Index[0] Is a placeholder
	for(const FAuraLevelUpInfo& Info : LevelUpInformation)
	{
		if(XP < Info.LevelUpRequirement) return Level;
		++Level;
	}
	return Level;
}
