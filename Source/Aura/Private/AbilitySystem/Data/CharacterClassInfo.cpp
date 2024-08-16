// Copyright QiuXuan


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefalutInfo UCharacterClassInfo::GetClassDefaultInfo(const ECharacterClass& InCharacterClass) const
{
	return CharacterClassInfomation.FindChecked(InCharacterClass);
}
