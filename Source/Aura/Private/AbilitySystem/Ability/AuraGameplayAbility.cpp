// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

#include "AuraAttributeSet.h"

FString UAuraGameplayAbility::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), TEXT("Default Ability Name - TioPlato"), AbilityLevel);
}

FString UAuraGameplayAbility::GetNextLevelDescription(const int32& AbilityLevel)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</>\n<Default>Causes much more damage</>"), AbilityLevel);
}

FString UAuraGameplayAbility::GetLockedDescription(const int32& AbilityLevel)
{
	return FString::Printf(TEXT("<Default>Ability Locked Until Level: %d</>"), AbilityLevel);
}

float UAuraGameplayAbility::GetManaCost(const int32& AbilityLevel) const
{
	float ManaCost = 0.f;
	if(const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for(const FGameplayModifierInfo& ModifierInfo : CostEffect->Modifiers)
		{
			if(ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(AbilityLevel, ManaCost);
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(const int32& AbilityLevel) const
{
	float Cooldown = 0.f;
	if(const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(AbilityLevel, Cooldown);
	}
	return Cooldown;
}