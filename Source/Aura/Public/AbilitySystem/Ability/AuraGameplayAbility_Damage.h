// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "CombatInterface.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Damage.generated.h"

// For Async Gameplay Tasks Call Back Usage
USTRUCT()
struct FDamageAbilityProperties
{
	GENERATED_BODY()

	FDamageAbilityProperties(){}

	UPROPERTY()
	AActor* AvatarActor = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;
	
	UPROPERTY()
	UAnimMontage* AttackMontage = nullptr;
	
	UPROPERTY()
	FGameplayTag AttackMontageTag = FGameplayTag();
	
	UPROPERTY()
	FGameplayTag AttackSocketTag = FGameplayTag();
};

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypeMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontagesArray) const;

	// For Async Tasks
	FDamageAbilityProperties DamageAbilityProperties;
	bool SetDamageAbilityProperties(FDamageAbilityProperties& OutProps);

	UFUNCTION()
	void MontageEndAbility();
};
