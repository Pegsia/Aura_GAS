// Copyright QiuXuan


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "CharacterClassInfo.h"
#include "CombatInterface.h"

struct AuraDamageStatic
{
	// Target
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); 
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	// Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDef;
	
	AuraDamageStatic()
	{
		// Target
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); // AttributeSet, Attr, AttributeCaptureSource, bSnapShot
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);		

		// Source
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);		
	}
	
	void InitializeTagsToCaptureDefsMap()
	{
		// FAuraGameplayTag Initialize After AuraDamageStatic, Manual Initialize it in AuraAssetManager
		TagsToCaptureDef.Empty();
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);		
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Resistance_Physical, PhysicalResistanceDef);

		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDef.Emplace(GameplayTags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
	}
};

static AuraDamageStatic& GetDamageStatic()
{
	static AuraDamageStatic DStatic; // Never go away
	return DStatic;
}

void UExecCalc_Damage::InitializeAuraDamageStaticMap()
{
	// 全局初始化之间的依赖关系，在AuraAssetManager中初始化
	GetDamageStatic().InitializeTagsToCaptureDefsMap();
}

/* ------------------------------------------------------------
*  ExecCalc
* ------------------------------------------------------------*/ 	
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Emplace(GetDamageStatic().ArmorDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().BlockChanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().FireResistanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().LightningResistanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().ArcaneResistanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().PhysicalResistanceDef);
	
	RelevantAttributesToCapture.Emplace(GetDamageStatic().ArmorPenetrationDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().CriticalHitChanceDef);
	RelevantAttributesToCapture.Emplace(GetDamageStatic().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Get Props
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	int32 SourcePlayerLevel = 1;
	if(SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if(TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = EffectSpec.GetContext();
	
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
		
	// Capture Attribute Magnitude 
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);
	
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().ArmorDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);	
	
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);
	
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	// Capture Coefficients
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float ArmorPenetrationCoefficients = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	const float EffectiveArmorCoefficients = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	const float CriticalHitResistanceCoefficients = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	/* ------------------------------------------------------------
	 * calculate Damage
	 * ------------------------------------------------------------*/ 	

	// Get Debuff
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	for(const TTuple<FGameplayTag, FGameplayTag>& Pair : AuraTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;		
		
		// Check if we are this kind of Damage Type
		float DamageMagnitude = EffectSpec.GetSetByCallerMagnitude(DamageType, false);
		if(DamageMagnitude == 0) continue;

		// Get Target Debuff Resistance
		float TargetDebuffResistance = 0;
		const FGameplayTag& TargetResistanceType = AuraTags.DamageTypesToResistance[DamageType];
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().TagsToCaptureDef[TargetResistanceType], EvaluateParameters, TargetDebuffResistance);
		TargetDebuffResistance = FMath::Max<float>(0, TargetDebuffResistance);

		// Calculate DebuffChance
		float SourceDebuffChance = EffectSpec.GetSetByCallerMagnitude(AuraTags.Debuff_Chance, false);
		float TargetDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;
		const bool bDebuff = FMath::RandRange(1, 100) < TargetDebuffChance;
		if(bDebuff)
		{
			float DebuffDamage = EffectSpec.GetSetByCallerMagnitude(AuraTags.Debuff_Damage, false);
			float DebuffFrequency = EffectSpec.GetSetByCallerMagnitude(AuraTags.Debuff_Frequency, false);
			float DebuffDuration = EffectSpec.GetSetByCallerMagnitude(AuraTags.Debuff_Duration, false);
			
			UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageType);
			UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, true);
			UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, DebuffDamage);
			UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, DebuffFrequency);
			UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, DebuffDuration);
		}		
	}
	
	// Get All Types of Set by caller Damage and Resistance 
	float Damage = 0;
	for(const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistance)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		checkf(GetDamageStatic().TagsToCaptureDef.Contains(ResistanceTag), TEXT("Tags To Capture Def doesn't contain Tag:[%s] in ExecCala_Damage"), *ResistanceTag.ToString());

		float DamageValue = EffectSpec.GetSetByCallerMagnitude(DamageTypeTag, false);
		if(DamageValue == 0) continue;
		
		float ResistanceValue = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().TagsToCaptureDef[ResistanceTag], EvaluateParameters, ResistanceValue);
		ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100.f);
		
		DamageValue *= (100.f - ResistanceValue) / 100.f;		
		Damage += DamageValue;
	}

	// Prevent further calc, otherwise Damage Can sometimes be SourceCriticalHitDamage in Line 236
	if(Damage == 0) return;
	
	// if block half the damage
	const bool bTargetBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bTargetBlocked);
	Damage = bTargetBlocked ? Damage / 2.f : Damage;		
	
	// ArmorPenetration ignores a percentage of Target's Armor
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficients) / 100.f;
	
	// Armor ignores a percentage of Incoming Damage
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficients) / 100.f;

	// CriticalHit Double the damage plus SourceCriticalHitDamage
	const float EffectiveCriticalHitChance = SourceCriticalHitChance  - TargetCriticalHitResistance * CriticalHitResistanceCoefficients;
	const bool bSourceCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bSourceCriticalHit);
	Damage = bSourceCriticalHit ? Damage * 2 + SourceCriticalHitDamage : Damage;

	if(TargetASC->HasMatchingGameplayTag(AuraTags.Abilities_Passive_HaloOfProtection))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Damage Before: %f"), Damage));
		Damage *= 0.8;		
	}
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage); //FGameplayAttribute(FProperty *NewProperty);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}


