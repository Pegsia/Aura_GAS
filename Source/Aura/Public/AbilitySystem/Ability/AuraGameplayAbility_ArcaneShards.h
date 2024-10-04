// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"
#include "AuraGameplayAbility_ArcaneShards.generated.h"


UCLASS()
class AURA_API UAuraGameplayAbility_ArcaneShards : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()
	
protected:	
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	int32 MaxArcaneShardsSpawnNumber= 11;
};
