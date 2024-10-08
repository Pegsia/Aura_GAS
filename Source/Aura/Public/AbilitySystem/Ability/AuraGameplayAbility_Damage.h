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

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetDamageAmountAtAbilityLevel() const;

	// For Debuffs
	UFUNCTION(BlueprintCallable)
	FDamageEffectProperties SetDamageEffectProperties(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockBackDirection = false,
		FVector KnockBackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat DamageAmount;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff", meta = (EditCondition = "DebuffChance != 0"))
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff", meta = (EditCondition = "DebuffChance != 0"))
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff", meta = (EditCondition = "DebuffChance != 0"))
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|KnockBack")
	float KnockBackChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage|KnockBack", meta = (EditCondition = "KnockBackChance != 0"))
	float KnockBackMagnitude = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Radial")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial", meta = (EditCondition = "bIsRadialDamage"))
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial", meta = (EditCondition = "bIsRadialDamage"))
	float RadialDamageOuterRadius = 0.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontagesArray) const;

	// For Async Tasks
	FDamageAbilityProperties DamageAbilityProperties;
	bool SetDamageAbilityProperties();
};
