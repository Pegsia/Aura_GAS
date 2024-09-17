// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_ProjectileSpell.h"
#include "CombatInterface.h"
#include "AuraProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
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
	if(SetDamageAbilityProperties(DamageAbilityProperties))
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

FString UAuraGameplayAbility_ProjectileSpell::GetCurrentLevelDescription(int32 AbilityLevel)
{
	const int32 Damage = DamageTypeMap[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(AbilityLevel);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, NumProjectiles);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
	const FString CurrentLevelString = FString::Printf(
		TEXT("<Title>Fire Bolt</>\n"
			"<Small>Level: </>"
			"<Level>%d</>\n\n"
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			"<Damage>%d </>"
			"<Default>fire damage with a chance to burn.</>"			
			),
			 AbilityLevel, *FireBoltString, Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_ProjectileSpell::GetNextLevelDescription(int32 AbilityLevel)
{
	const int32 PreDamage = DamageTypeMap[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(AbilityLevel - 1);
	const int32 Damage = DamageTypeMap[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(AbilityLevel);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, NumProjectiles);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
	const FString NextLevelString = FString::Printf(
		TEXT("<Title>NEXT LEVEL</>\n"
			"<Small>Level: </>"
			"<Gray>%d -> </>"
			"<Level>%d</>\n\n"
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			"<Gray>%d -> </>"
			"<Damage>%d </>"
			"<Default>fire damage with a chance to burn.</>"			
			),
			AbilityLevel - 1, AbilityLevel, *FireBoltString, PreDamage, Damage);
	return NextLevelString;
}
