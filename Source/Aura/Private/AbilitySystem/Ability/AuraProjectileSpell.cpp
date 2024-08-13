// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CombatInterface.h"
#include "AuraProjectile.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility (C++)"), true, true, FLinearColor::Yellow, 3.f);

	if (!HasAuthority(&ActivationInfo)) return; // Only on Server

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
