// Copyright QiuXuan

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

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

// For Damage Effect Magnitude Usage
USTRUCT(BlueprintType)
struct FDamageEffectProperties
{
	GENERATED_BODY()

	FDamageEffectProperties(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY()
	float AbilityLevel = 1.f;
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float BaseDamage = 0.f;

	UPROPERTY()
	float DebuffChance = 0.f;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayEffectContext::StaticStruct();
	}

	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	bool GetIsBlockedHit() const { return bIsBlockedHit; }
	bool GetIsCriticalHit() const { return bIsCriticalHit; }
	bool GetIsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	float GetDebuffDuration() const { return DebuffDuration; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }

	void SetIsBlockedHit(const bool& bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(const bool& bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsSuccessfulDebuff(const bool& bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(const float& InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffFrequency(const float& InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDebuffDuration(const float& InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	TSharedPtr<FGameplayTag> DamageType = nullptr;
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};