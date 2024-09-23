// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "CombatInterface.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_Damage : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ApplyAllTypeOfDamage(AActor* TargetActor); 
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat DamageAmount;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockBackChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockBackMagnitude = 60.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontagesArray) const;

	// For Async Tasks
	FDamageAbilityProperties DamageAbilityProperties;
	bool SetDamageAbilityProperties();

	// For Debuffs
	FDamageEffectProperties SetDamageEffectProperties(const AActor* TargetActor = nullptr) const;
	
	UFUNCTION()
	void MontageEndAbility();
};
