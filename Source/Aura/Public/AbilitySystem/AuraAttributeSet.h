// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * struct stores useful properties in PostGameplayEffectExecute
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;
	
	// Effect Source
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	// Effect Target
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;	
};

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAuraAttributeSet();

	// 其他实现方法见最后
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributesFuncPtr;
	
	//~ Begin UAttributeSet Interface
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//~ End UAttributeSet Interface

	void RefillVitalAttributes();
private:
	// Handle PostGameplay Attribute
	void HandleIncomingDamage(const FEffectProperties& Props);
	void HandleDebuff(const FEffectProperties& Props);
	void HandleIncomingXP(const FEffectProperties& Props);
	
	// Store Effect Properties when a GameplayEffect is applied
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutProps) const;
	void ShowFloatingDamage(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit);
	void SendXPEvent(const FEffectProperties& Props);

	// for Refill Vital Attributes
	bool bRestoreHealth = false;
	bool bRestoreMana = false;
	
/* ---------------------------------------------------------------
	FGameplayAttributeData and Corresponding Boiler Plate
-----------------------------------------------------------------*/
public:
	/**
	 * FGameplayAttributeData
	 * Meat  Attributes
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Meat Attributes")
	FGameplayAttributeData IncomingXP;
	
	/**
	 * Vital  Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Health", Category = "Vital Attributes")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Mana", Category = "Vital Attributes")
	FGameplayAttributeData Mana;	

	/**
	 * Primary  Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Strength", Category = "Primary Attributes")
	FGameplayAttributeData Strength;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Intelligence", Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Resilience", Category = "Primary Attributes")
	FGameplayAttributeData Resilience;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Vigor", Category = "Primary Attributes")
	FGameplayAttributeData Vigor;

	/**
	 * Secondary Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_Armor", Category = "Secondary Attributes")
	FGameplayAttributeData Armor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_ArmorPenetration", Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_BlockChance", Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_CriticalHitChance", Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_CriticalHitDamage", Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_CriticalHitResistance", Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_HealthRegeneration", Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_ManaRegeneration", Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_MaxHealth", Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_MaxMana", Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;

	/**
	 * Damage Resistance
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_FireResistance", Category = "Rsistance Attributes")
	FGameplayAttributeData FireResistance;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_LightningResistance", Category = "Rsistance Attributes")
	FGameplayAttributeData LightningResistance;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_ArcaneResistance", Category = "Rsistance Attributes")
	FGameplayAttributeData ArcaneResistance;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_PhysicalResistance", Category = "Rsistance Attributes")
	FGameplayAttributeData PhysicalResistance;
	
	/**
	 * ReplicatedUsing
	 * Vital  Attributes
	 */
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
	/**
	 * Primary  Attributes
	 */
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	/**
	 * Secondary Attributes
	 */
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	/**
	 * Damage Resistance
	 */
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;
	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;
	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
	
	/**
	 * Helper Accessors
	 * Meta  Attributes
	 */
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXP);

	/**
	 * Vital  Attributes
	 */
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	
	/**
	 * Primary  Attributes
	 */
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

	/**
	 * Secondary Attributes
	 */
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	/**
	 * Damage Resistance
	 */
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance);
};

/**
*	实现函数指针的几种方法
*	1. Delegate
*	DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature)
*	TMap<FGameplayTag, FAttributeSignature> TagsToAttributes;
* 
*	Delegate 也是存储C++函数指针，可以不用UE的Delegate包装
*	FAttributeSignature StrengthDelegate;
*	StrengthDelegate.BindStatic(GetStrengthAttribute);
*	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, StrengthDelegate);
* 
*	2. UE的FuncPtr
*	TMap<FGameplayTag, TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr> TagsToAttributes;
*	
*	3. Raw C++的FuncPtr
*	TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToAttributesFunc;
*  
*	4. 重命名的指针种类，注意按照UE命名标准，需要在类型前加入前缀，如这里的F
*	typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
*	TMap<FGameplayTag, FAttributeFuncPtr> TagsToAttributesFunc;
*  
*	5. Modern C++的模板重命名，因为是模板这里前缀选择T
*	template<class T>
*	using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;
*	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributesFuncPtr;
* 
*	其他类型举例
*	TStaticFuncPtr<float(int32, float, double)> RandomFunctionPtr;
*	static float RandomFunction(int32 I, float f, double d);
* 
*	RandomFunctionPtr = RandomFunction;
*	float F = RandomFunctionPtr(1, 2.f, 3.f);
* 
*	float(*RawFuncPtr)(int32, float, double) = RandomFunction;
*	float F = RawFuncPtr(1, 2.f, 3.f);
*/