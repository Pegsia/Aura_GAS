// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Passive.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_Passive : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceivedDeactivate(const FGameplayTag& AbilityTag);
};
