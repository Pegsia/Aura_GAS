// Copyright QiuXuan


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(const ECharacterClass& InCharacterClass) const
{
	return CharacterClassInformation.FindChecked(InCharacterClass);
}
