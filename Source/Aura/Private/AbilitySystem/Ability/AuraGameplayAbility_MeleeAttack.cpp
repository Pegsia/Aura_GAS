// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

static TAutoConsoleVariable<bool> CVarDrawDebugMeleeSphere(TEXT("aura.MeleeSphere"), false, TEXT("DrawDebugMeleeSphere for Enemy Attack"), ECVF_Cheat);

void UAuraGameplayAbility_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(SetDamageAbilityProperties())
	{
		bHasHitTarget = false; // this Ability Instanced Per Actor, ensure Initialize bool correctly
		ICombatInterface::Execute_SetFacingWarpTarget(DamageAbilityProperties.AvatarActor, DamageAbilityProperties.TargetActor->GetActorLocation());
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), DamageAbilityProperties.AttackMontage);
		UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageAbilityProperties.AttackMontageTag);

		GameplayEventTask->OnlyTriggerOnce = true;
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
	FVector CombatSockedLocation = ICombatInterface::Execute_GetCombatSocketLocation(DamageAbilityProperties.AvatarActor, DamageAbilityProperties.AttackSocketTag);

	TArray<AActor*> OverlappedActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		DamageAbilityProperties.AvatarActor,
		OverlappedActors,
		TArray(&DamageAbilityProperties.AvatarActor, 1),
		CombatSockedLocation,
		Radius);

	if(CVarDrawDebugMeleeSphere.GetValueOnGameThread())
	{
		DrawDebugSphere(DamageAbilityProperties.AvatarActor->GetWorld(),CombatSockedLocation,Radius,16, FColor::Cyan,false, 3.f);
	}
	
	for(AActor* OtherActor : OverlappedActors)
	{
		if(!UAuraAbilitySystemLibrary::IsFriend(OtherActor, DamageAbilityProperties.AvatarActor))
		{
			UAuraAbilitySystemLibrary::ApplyDamageEffect(SetDamageEffectProperties(OtherActor));
			// ApplyAllTypeOfDamage(OtherActor);
			bHasHitTarget = true;
		}
	}

	if(bHasHitTarget)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
		FGameplayCueParameters CueParameters;
		CueParameters.Location = CombatSockedLocation;
		CueParameters.EffectCauser = DamageAbilityProperties.AvatarActor;
		CueParameters.SourceObject = DamageAbilityProperties.TargetActor;
		CueParameters.AggregatedSourceTags.AddTag(DamageAbilityProperties.AttackMontageTag);
		AbilitySystemComponent->ExecuteGameplayCue(MeleeImpactCueTag, CueParameters);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);	
}

