// Copyright QiuXuan


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByAbilityTag(const FGameplayTag& AbilityTag, const bool bLogIfNotFound) const
{
	for(const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if(Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}
	if(bLogIfNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
