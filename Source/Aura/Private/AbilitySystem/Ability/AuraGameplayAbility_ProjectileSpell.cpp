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

	/*
	 * if (bHasBlueprintActivate) K2_ActivateAbility(); 只要蓝图里有ActivateAbility节点就会先调用蓝图版本随后才调用C++
	 */
	if(SetDamageAbilityProperties())
	{
		// Enemy has a BP implementation
		if(DamageAbilityProperties.AvatarActor->ActorHasTag(ACTOR_TAG_ENEMY)) return;		

		CommitAbility(Handle, ActorInfo, ActivationInfo); // Fail时甚至不会触发ActivateAbility
		
		UTargetDataUnderMouse* TargetData = UTargetDataUnderMouse::CreateTargetDataUnderMouse(this);
		TargetData->OnValidDataReceived.AddDynamic(this, &UAuraGameplayAbility_ProjectileSpell::TargetDataReceived);
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

	GameplayEventTask->OnlyTriggerOnce = true;
	GameplayEventTask->EventReceived.AddDynamic(this, &UAuraGameplayAbility_ProjectileSpell::FireBoltEventReceived);
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UAuraGameplayAbility_ProjectileSpell::MontageEndAbility);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UAuraGameplayAbility_ProjectileSpell::MontageEndAbility);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UAuraGameplayAbility_ProjectileSpell::MontageEndAbility);
		
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
		
		Projectile->DamageEffectProperties = SetDamageEffectProperties();		
		Projectile->FinishSpawning(SpawnTransform);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}