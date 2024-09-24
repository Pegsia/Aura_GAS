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
	
	virtual void ProjectileEventReceived(FGameplayEventData Payload) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, AActor* HomingTarget);

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 ProjectileNumber = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 MaxProjectileNumber = 5;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	bool bLaunchHomingProjectiles = true;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt", meta = (EditCondition = "bLaunchHomingProjectiles"))
	float MinHomingAcceleration = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt", meta = (EditCondition = "bLaunchHomingProjectiles"))
	float MaxHomingAcceleration = 3200.f;
	
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface
};
