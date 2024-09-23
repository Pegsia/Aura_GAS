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
	
	virtual void FireBoltEventReceived(FGameplayEventData Payload) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, AActor* HomingTarget, bool bOverridePitch = false, float PitchOverride = 0.f);

	UPROPERTY(VisibleAnywhere, Category = "FireBolt")
	int32 ProjectileNumber = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 MaxProjectileNumber = 5;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;
	
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface
};
