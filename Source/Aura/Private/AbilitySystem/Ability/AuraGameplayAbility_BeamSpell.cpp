// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_BeamSpell.h"

#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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
	UAuraAbilitySystemLibrary::GetClosestTargets(Origin, AdditionalTargetNumber, OverlappingActors, AdditionalTargets);
}
