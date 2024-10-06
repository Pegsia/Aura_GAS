// Copyright QiuXuan


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "Components/AudioComponent.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValidOverlapped(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectProperties.DeathImpulse = GetActorForwardVector() * DamageEffectProperties.DeathImpulseMagnitude;
			DamageEffectProperties.TargetASC = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectProperties);
		}
	}
}

void AAuraFireBall::OnHit()
{
	bHit = true;
	if(LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if(GetInstigator())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetInstigator(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	
}
