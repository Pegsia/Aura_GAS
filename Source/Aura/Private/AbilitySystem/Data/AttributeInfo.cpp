// Copyright QiuXuan


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& InAttributeTag, bool bLogNotFound /*= false*/) const
{
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(InAttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant't find AttributeTag {%s} on AttributeInfo {%s}"), *InAttributeTag.ToString(), *GetNameSafe(this));
	}

	return FAuraAttributeInfo();
}
