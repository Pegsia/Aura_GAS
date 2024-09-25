// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnemyInterface.h"

void UAuraGameplayAbility_Damage::ApplyAllTypeOfDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float Damage = DamageAmount.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, Damage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraGameplayAbility_Damage::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontagesArray) const
{
	if(TaggedMontagesArray.IsEmpty()) return FTaggedMontage();
	int32 RandomIndex = FMath::RandRange(0, TaggedMontagesArray.Num() - 1);
	return TaggedMontagesArray[RandomIndex];
}

bool UAuraGameplayAbility_Damage::SetDamageAbilityProperties()
{
	DamageAbilityProperties.AvatarActor = CurrentActorInfo->AvatarActor.Get();
	if(DamageAbilityProperties.AvatarActor->Implements<UEnemyInterface>())
	{
		DamageAbilityProperties.TargetActor = IEnemyInterface::Execute_GetCombatTarget(DamageAbilityProperties.AvatarActor);	
	}
	
	if(DamageAbilityProperties.AvatarActor->Implements<UCombatInterface>())
	{
		// Get Attack Props
		FTaggedMontage TaggedMontage = GetRandomTaggedMontageFromArray(ICombatInterface::Execute_GetAttackMontages(DamageAbilityProperties.AvatarActor));		
		DamageAbilityProperties.AttackMontage = TaggedMontage.Montage;
		DamageAbilityProperties.AttackMontageTag = TaggedMontage.MontageTag;
		DamageAbilityProperties.AttackSocketTag = TaggedMontage.SocketTag;
		
		return true;
	}
	return false;
}

FDamageEffectProperties UAuraGameplayAbility_Damage::SetDamageEffectProperties(const AActor* TargetActor) const
{
	FDamageEffectProperties DamageEffectProperties;
	DamageEffectProperties.WorldContextObject = GetAvatarActorFromActorInfo();
	DamageEffectProperties.SourceASC = GetAbilitySystemComponentFromActorInfo();
	DamageEffectProperties.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	DamageEffectProperties.AbilityLevel = GetAbilityLevel();
	DamageEffectProperties.DamageEffectClass = DamageEffectClass;
	DamageEffectProperties.DamageType = DamageType;
	DamageEffectProperties.BaseDamage = DamageAmount.GetValueAtLevel(DamageEffectProperties.AbilityLevel);
	DamageEffectProperties.DebuffChance = DebuffChance;
	DamageEffectProperties.DebuffDamage = DebuffDamage;
	DamageEffectProperties.DebuffFrequency = DebuffFrequency;
	DamageEffectProperties.DebuffDuration = DebuffDuration;
	DamageEffectProperties.DeathImpulseMagnitude = DeathImpulseMagnitude;
	DamageEffectProperties.KnockBackChance = KnockBackChance;
	DamageEffectProperties.KnockBackMagnitude = KnockBackMagnitude;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		const FVector ToTarget = Rotation.Vector();
		DamageEffectProperties.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		DamageEffectProperties.KnockBackVector = ToTarget * KnockBackMagnitude;
	}
	return DamageEffectProperties;
}

void UAuraGameplayAbility_Damage::MontageEndAbility()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
