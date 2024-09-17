// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "AuraGameplayAbility.generated.h"

/**
 *  Base Aura Ability Class
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetCurrentLevelDescription(int32 AbilityLevel);
	virtual FString GetNextLevelDescription(int32 AbilityLevel);
	static FString GetLockedDescription(int32 AbilityLevel);
};
