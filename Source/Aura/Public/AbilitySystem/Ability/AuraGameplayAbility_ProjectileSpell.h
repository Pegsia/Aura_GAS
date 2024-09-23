// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility_Damage.h"
#include "AuraGameplayAbility_ProjectileSpell.generated.h"

class AAuraProjectile;
/**
 * GA That Spawn Projectile Spell
 */
UCLASS()
class AURA_API UAuraGameplayAbility_ProjectileSpell : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& ProjectileTargetLocation, bool bOverridePitch = false, float PitchOverride = 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UFUNCTION()
	void TargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	UFUNCTION()
	virtual void FireBoltEventReceived(FGameplayEventData Payload);

	FVector FireBoltHitLocation{};
};
