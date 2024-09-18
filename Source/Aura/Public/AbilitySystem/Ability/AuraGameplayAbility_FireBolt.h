// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_ProjectileSpell.h"
#include "AuraGameplayAbility_FireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_FireBolt : public UAuraGameplayAbility_ProjectileSpell
{
	GENERATED_BODY()

protected:
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface
};
