// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_ProjectileSpell.h"
#include "CombatInterface.h"
#include "AuraProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TargetDataUnderMouse.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Aura/Aura.h"

void UAuraGameplayAbility_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(SetDamageAbilityProperties(DamageAbilityProperties))
	{
		// Enemy has a BP implementation
		if(DamageAbilityProperties.AvatarActor->ActorHasTag(ACTOR_TAG_ENEMY)) return;		
		
		UTargetDataUnderMouse* TargetData = UTargetDataUnderMouse::CreateTargetDataUnderMouse(this);
		TargetData->OnValidDataReceived.AddDynamic(this, &ThisClass::TargetDataReceived);
		TargetData->Activate();
	}
}

void UAuraGameplayAbility_ProjectileSpell::TargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	// Get Hit Location
	FireBoltHitLocation =  UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(DataHandle, 0).Location;
	ICombatInterface::Execute_SetFacingWarpTarget(DamageAbilityProperties.AvatarActor, FireBoltHitLocation);
	
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), DamageAbilityProperties.AttackMontage);
	UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageAbilityProperties.AttackMontageTag);
		
	GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::FireBoltEventReceived);
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::MontageEndAbility);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::MontageEndAbility);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::MontageEndAbility);
		
	PlayMontageAndWaitTask->Activate();
	GameplayEventTask->Activate();
}

void UAuraGameplayAbility_ProjectileSpell::FireBoltEventReceived(FGameplayEventData Payload)
{
	// Montage AttackTag Received
	SpawnProjectile(FireBoltHitLocation);
}

void UAuraGameplayAbility_ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, bool bOverridePitch, float PitchOverride)
{
	// Only on Server
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	
	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		const FVector SpawnLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), DamageAbilityProperties.AttackSocketTag);
		FRotator SpawnRotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		if(bOverridePitch)
		{
			SpawnRotation.Pitch = PitchOverride;
		}
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());		

		// Spawn Projectile Deferred
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);		

		// Assign Damage By Damage Type
		for(const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypeMap)
		{
			const float Damage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
			// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Damage: %s Causes: %f Damage"), *Pair.Key.ToString(), Damage));
		}		
		
		Projectile->DamageEffectSpecHandle = SpecHandle;		
		Projectile->FinishSpawning(SpawnTransform);
	}

	// UKismetSystemLibrary::Delay()
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


