// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_FireBlast.h"

#include "AuraAbilitySystemLibrary.h"
#include "AuraFireBall.h"


TArray<AAuraFireBall*> UAuraGameplayAbility_FireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;

	const FVector& ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector& ActorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(ForwardVector, 360.f, NumFireBolts);

	for(const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ActorLocation);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FireBall->DamageEffectProperties = SetDamageEffectProperties();
		FireBall->FinishSpawning(SpawnTransform);

		FireBalls.Emplace(FireBall);
	}
	
	return FireBalls;
}

FString UAuraGameplayAbility_FireBlast::GetCurrentLevelDescription(const int32& AbilityLevel)
{
	//GetAbilityLevel(); Not Activate, NonValid CurrentActorInfo
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	
	const FString CurrentLevelString = FString::Printf(TEXT(
			// Title
			"<Title>Fire Blast</>\n"
			// AbilityLevel
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// Description
			"<Default>Launches %d fire balls in all direction, each coming back and </>"
			"<Default>exploding upon return, causing  </>"
			//Damage
			"<Damage>%d </><Default>radial fire damage with a chance to burn.</>"),
			 AbilityLevel,
			 ManaCost,
			 Cooldown,
			 NumFireBolts,
			 Damage);
	return CurrentLevelString;
}

FString UAuraGameplayAbility_FireBlast::GetNextLevelDescription(const int32& AbilityLevel)
{
	const float PreManaCost = FMath::Abs(GetManaCost(AbilityLevel - 1));
	const float ManaCost = FMath::Abs(GetManaCost(AbilityLevel));
	const float PreCooldown = FMath::Abs(GetCooldown(AbilityLevel - 1));
	const float Cooldown = FMath::Abs(GetCooldown(AbilityLevel));
	const int32 PreDamage = DamageAmount.GetValueAtLevel(AbilityLevel - 1);
	const int32 Damage = DamageAmount.GetValueAtLevel(AbilityLevel);
	
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
			"<Default>Launches %d fire balls in all direction, each coming back and </>"
			"<Default>exploding upon return, causing  </>"
			//Damage
			"<Gray>%d -> </><Damage>%d </><Default>radial fire damage with a chance to burn.</>"),
			AbilityLevel - 1, AbilityLevel,
			PreManaCost, ManaCost,
			PreCooldown, Cooldown,
			NumFireBolts,
			PreDamage, Damage);
	return NextLevelString;
}