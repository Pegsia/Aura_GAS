// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Summon.generated.h"

class AAuraCharacterBase;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_Summon : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraGameplayAbility_Summon();
	
	UFUNCTION(BlueprintCallable, Category = "Summon")
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure, Category = "Summon")
	AAuraCharacterBase* GetRandomMinionClassCDO();

	UFUNCTION(BlueprintCallable, Category = "Summon")
	AAuraCharacterBase* SpawnMinionAtLocation(const FVector& SpawnLocation);
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	int32 NumMinion = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float SpawnSpread = 90.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float MinSpawnDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float MaxSpawnDistance = 400.f;

private:
	void DebugDrawSummon(const FVector& Start, const FVector& End, const FColor& Color, const AActor* Avatar);
};
