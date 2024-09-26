// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_BeamSpell.h"

#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraGameplayAbility_BeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if(HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.Location;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraGameplayAbility_BeamSpell::StoreOwnerPlayerProperties()
{
	if(CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraGameplayAbility_BeamSpell::TraceFirstTarget(const FVector& BeamTarget)
{
	check(OwnerCharacter);
	if(OwnerCharacter->Implements<UCombatInterface>())
	{
		const FVector& CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(OwnerCharacter, FAuraGameplayTags::Get().CombatSocket_Weapon);

		TArray<AActor*> ActorsToIgnore{OwnerCharacter};
		FHitResult HitResult;
		UKismetSystemLibrary::SphereTraceSingle(
			OwnerCharacter,
			CombatSocketLocation,
			BeamTarget,
			10.f,
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true);

		if(HitResult.bBlockingHit)
		{
			MouseHitLocation = HitResult.Location;
			MouseHitActor = HitResult.GetActor();
		}

		if(TScriptInterface<ICombatInterface> FirstCombatInterface = MouseHitActor)
		{
			if(!FirstCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraGameplayAbility_BeamSpell::FirstTargetDead))
			{
				FirstCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraGameplayAbility_BeamSpell::FirstTargetDead);
			}
		}
	}
}

void UAuraGameplayAbility_BeamSpell::GetAdditionalTargets(TArray<AActor*>& AdditionalTargets)
{
	const TArray<AActor*> IgnoreActors{OwnerCharacter, MouseHitActor};

	// Find Live Combat Target Within Radius
	const FVector& Origin = MouseHitActor->GetActorLocation();
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		OwnerCharacter,
		OverlappingActors,
		IgnoreActors,
		Origin,
		AdditionalRadius.GetValueAtLevel(GetAbilityLevel()));

	// Find AdditionalTargetNumber Closest Combat Target
	int32 TargetNumber = FMath::Min(AdditionalTargetNumber, GetAbilityLevel() - 1);
	UAuraAbilitySystemLibrary::GetClosestTargets(Origin, TargetNumber, OverlappingActors, AdditionalTargets);

	for (AActor* Target : AdditionalTargets)
	{
		if (TScriptInterface<ICombatInterface> AdditionalCombatInterface = Target)
		{
			if (!AdditionalCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraGameplayAbility_BeamSpell::AdditionalTargetDead))
			{
				AdditionalCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraGameplayAbility_BeamSpell::AdditionalTargetDead);
			}
		}
	}
}

void UAuraGameplayAbility_BeamSpell::CommitCooldownAndEndAbility()
{
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	CurrentActorInfo->AbilitySystemComponent->NotifyAbilityCommit(this);
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
