// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_MeleeAttack.h"

#include "AuraAbilitySystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/GameplayStatics.h"

void UAuraGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(SetDamageAbilityProperties(DamageAbilityProperties))
	{
		bHasHitTarget = false; // this Ability Instanced Per Actor, ensure Initialize bool correctly
		ICombatInterface::Execute_SetFacingWarpTarget(DamageAbilityProperties.AvatarActor, DamageAbilityProperties.TargetActor->GetActorLocation());
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), DamageAbilityProperties.AttackMontage);
		UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageAbilityProperties.AttackTag);
		
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::MeleeEventReceived);
		PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::MontageEndAbility);
		PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::MontageEndAbility);
		PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::MontageEndAbility);
		
		PlayMontageAndWaitTask->Activate();
		GameplayEventTask->Activate();
	}
}

void UAuraGameplayAbility_MeleeAttack::MeleeEventReceived(FGameplayEventData Payload)
{
	// Get Overlapped Actors Within Socked Location Radius And Apply Damage To Enemies
	FVector CombatSockedLocation = ICombatInterface::Execute_GetCombatSocketLocation(DamageAbilityProperties.AvatarActor, DamageAbilityProperties.AttackTag);

	TArray<AActor*> OverlappedActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		DamageAbilityProperties.AvatarActor,
		OverlappedActors,
		TArray(&DamageAbilityProperties.AvatarActor, 1),
		CombatSockedLocation,
		Radius);

	for(AActor* OtherActor : OverlappedActors)
	{
		if(!UAuraAbilitySystemLibrary::IsFriend(OtherActor, DamageAbilityProperties.AvatarActor))
		{
			ApplyAllTypeOfDamage(OtherActor);
			// Spawn Niagara Per Hit
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(DamageAbilityProperties.AvatarActor,	DamageAbilityProperties.BloodEffect, CombatSockedLocation);
			bHasHitTarget = true;
		}
	}

	if(bHasHitTarget) // Only Play Sound Once
	{
		UGameplayStatics::PlaySoundAtLocation(DamageAbilityProperties.AvatarActor, DamageAbilityProperties.ImpactSound, CombatSockedLocation);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

