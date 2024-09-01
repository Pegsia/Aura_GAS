// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"
#include "AuraGameplayAbility_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_MeleeAttack : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void MeleeEventReceived(FGameplayEventData Payload);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Radius = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FGameplayTag MeleeImpactCueTag;
	
	bool bHasHitTarget = false;
};
