// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnemyInterface.h"

void UAuraGameplayAbility_Damage::ApplyAllTypeOfDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	for(TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypeMap)
	{
		const float Damage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Damage);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraGameplayAbility_Damage::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontagesArray) const
{
	if(TaggedMontagesArray.IsEmpty()) return FTaggedMontage();
	int32 RandomIndex = FMath::RandRange(0, TaggedMontagesArray.Num() - 1);
	return TaggedMontagesArray[RandomIndex];
}

bool UAuraGameplayAbility_Damage::SetDamageAbilityProperties(FDamageAbilityProperties& OutProps)
{
	OutProps.AvatarActor = CurrentActorInfo->AvatarActor.Get();
	if(OutProps.AvatarActor->Implements<UEnemyInterface>())
	{
		OutProps.TargetActor = IEnemyInterface::Execute_GetCombatTarget(OutProps.AvatarActor);	
	}
	
	if(OutProps.AvatarActor->Implements<UCombatInterface>())
	{
		// Get Attack Props
		FTaggedMontage TaggedMontage = GetRandomTaggedMontageFromArray(ICombatInterface::Execute_GetAttackMontages(OutProps.AvatarActor));		
		OutProps.AttackMontage = TaggedMontage.Montage;
		OutProps.AttackTag = TaggedMontage.MontageTag;
		OutProps.ImpactSound = TaggedMontage.ImpactSound;
		OutProps.BloodEffect = ICombatInterface::Execute_GetBloodEffect(OutProps.AvatarActor);
		
		return true;
	}
	return false;
}

void UAuraGameplayAbility_Damage::MontageEndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
