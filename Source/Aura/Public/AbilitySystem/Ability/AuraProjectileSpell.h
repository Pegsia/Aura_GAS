// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
/**
 * GA That Spawn Projectile Spell
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void SpawnFireBolt(const FVector& ProjectileTargetLocation);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
