// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_BeamSpell.h"

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

void UAuraGameplayAbility_BeamSpell::StoreOwnerPlayerController()
{
	if(CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
	}
}
