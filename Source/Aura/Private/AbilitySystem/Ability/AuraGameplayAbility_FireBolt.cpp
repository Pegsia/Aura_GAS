// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_FireBolt.h"

#include "AuraAbilitySystemLibrary.h"
#include "AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UAuraGameplayAbility_FireBolt::ProjectileEventReceived(FGameplayEventData Payload)
{
	ProjectileNumber = FMath::Min(GetAbilityLevel(), MaxProjectileNumber);
	SpawnProjectiles(ProjectileHitResult.Location, ProjectileHitResult.GetActor());
}

void UAuraGameplayAbility_FireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, AActor* HomingTarget)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	TRACE_BOOKMARK(TEXT("Fire Bolt"));
	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		const FVector SpawnLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), DamageAbilityProperties.AttackSocketTag);
		FRotator SpawnRotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		if(bOverridePitch) SpawnRotation.Pitch = PitchOverride;

		const FVector ForwardVector = SpawnRotation.Vector();

		const TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(ForwardVector, ProjectileSpread, ProjectileNumber);

		// Spawn Projectiles for all Rotation
		for(const FRotator& Rotator : Rotators)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SpawnLocation);
			SpawnTransform.SetRotation(Rotator.Quaternion());

			// Spawn Projectile Deferred
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
				ProjectileClass,
				SpawnTransform,
				GetOwningActorFromActorInfo(),
				Cast<APawn>(GetAvatarActorFromActorInfo()),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		
			Projectile->DamageEffectProperties = SetDamageEffectProperties();

			// Homing Target is Combat Character
			if(HomingTarget && HomingTarget->Implements<UCombatInterface>())
			{
				Projectile->MovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				// HomingTargetComponent is Weak Pointer, mark HomingTargetSceneComponent as UPROPERTY(), let UE Handles GC
				HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
				HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
				Projectile->MovementComponent->HomingTargetComponent = HomingTargetSceneComponent;			
			}
			Projectile->MovementComponent->HomingAccelerationMagnitude = FMath::FRandRange(MinHomingAcceleration, MaxHomingAcceleration);
			Projectile->MovementComponent->bIsHomingProjectile = bLaunchHomingProjectiles;
			
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}

FString UAuraGameplayAbility_FireBolt::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	//GetAbilityLevel(); Not Activate, NonValid CurrentActorInfo
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, MaxProjectileNumber);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Fire Bolt</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			//Damage
			"<Damage>%d </><Default>fire damage with a chance to burn.</>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown,
			 *FireBoltString,
			 Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_FireBolt::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 PreDamage = DamageAmount.GetValueAtLevel(AbilityLevel - 1);
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	const int32 FireBoltAmount = FMath::Min(AbilityLevel, MaxProjectileNumber);
	const FString FireBoltString = FireBoltAmount == 1 ? FString(TEXT("a bolt")) : FString::Printf(TEXT("%d bolts"), FireBoltAmount);
	
	const FString NextLevelString = FString::Printf(TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"
			// AbilityLevel
			"<Small>Level: </><Gray>%d -> </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><Gray>%.1f -> </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Gray>%.1f -> </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Launches %s of fire, exploding on impact and dealing: </>"
			//Damage
			"<Gray>%d -> </><Damage>%d </><Default>fire damage with a chance to burn.</>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown,
			*FireBoltString,
			PreDamage, Damage);
	return NextLevelString;
}
