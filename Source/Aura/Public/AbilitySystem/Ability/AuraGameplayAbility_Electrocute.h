// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_BeamSpell.h"
#include "AuraGameplayAbility_Electrocute.generated.h"

UCLASS()
class AURA_API UAuraGameplayAbility_Electrocute : public UAuraGameplayAbility_BeamSpell
{
	GENERATED_BODY()

protected:	
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface
};
