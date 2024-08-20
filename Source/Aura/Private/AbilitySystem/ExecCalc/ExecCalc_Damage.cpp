// Copyright QiuXuan


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"

struct AuraDamageStatic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); // Same as MMCs
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	AuraDamageStatic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); // AttributeSet, Attr, AttributeCaptureSource, bSnapShot
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
	}
};

static const AuraDamageStatic& GetDamageStatic()
{
	static AuraDamageStatic DStatic; // Never go away
	return DStatic;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageStatic().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageStatic().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Get Damage Set by caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// Capture BlockChance on Target, if block half the damage
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatic().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	
	const bool bTargetBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	Damage = bTargetBlocked ? Damage / 2.f : Damage;
	
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage); //FGameplayAttribute(FProperty *NewProperty);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
