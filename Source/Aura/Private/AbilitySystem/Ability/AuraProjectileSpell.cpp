// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CombatInterface.h"
#include "AuraProjectile.h"
#include "../Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility (C++)"), true, true, FLinearColor::Yellow, 3.f);
	
	UAbilityTask_WaitGameplayEvent* FireBoltEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Montage.FireBolt"));
	FireBoltEvent->Activate();
	FireBoltEvent->EventReceived.AddDynamic(this, &ThisClass::SpawnFireBolt2);
}

void UAuraProjectileSpell::SpawnFireBolt()
{
	// Only on Server
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	TScriptInterface<ICombatInterface> CombatInterface = GetAvatarActorFromActorInfo();
	if (CombatInterface)
	{
		const FVector SpawnLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		// TODO: Set Spawn Rotation

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		// TODO: Give the projectile some GE
		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UAuraProjectileSpell::SpawnFireBolt2(FGameplayEventData Payload)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	TScriptInterface<ICombatInterface> CombatInterface = GetAvatarActorFromActorInfo();
	if (CombatInterface)
	{
		const FVector SpawnLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		// TODO: Set Spawn Rotation

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		// TODO: Give the projectile some GE
		Projectile->FinishSpawning(SpawnTransform);
	}
}
