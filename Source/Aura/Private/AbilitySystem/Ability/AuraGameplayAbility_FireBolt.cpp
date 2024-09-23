// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_FireBolt.h"

#include "Kismet/KismetSystemLibrary.h"

void UAuraGameplayAbility_FireBolt::FireBoltEventReceived(FGameplayEventData Payload)
{
	ProjectileNumber = FMath::Min(GetAbilityLevel(), MaxProjectileNumber);
	if(ProjectileNumber > 1) SpawnProjectiles(FireBoltHitLocation, nullptr);
	else SpawnProjectile(FireBoltHitLocation);	
}

void UAuraGameplayAbility_FireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, AActor* HomingTarget, bool bOverridePitch, float PitchOverride)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	
	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		SetDamageAbilityProperties();
		const FVector SpawnLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), DamageAbilityProperties.AttackSocketTag);
		FRotator SpawnRotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		if(bOverridePitch) SpawnRotation.Pitch = PitchOverride;

		const FVector ForwardVector = SpawnRotation.Vector();
		const FVector LeftVector = ForwardVector.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
		const FVector RightVector = ForwardVector.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);		

		const float DeltaSpread = ProjectileSpread / (ProjectileNumber - 1);
		for(int i = 0; i < ProjectileNumber; ++i)
		{
			const FVector Direction = LeftVector.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			const FVector Start = SpawnLocation + FVector(0,0,5);
			UKismetSystemLibrary::DrawDebugArrow(
				GetAvatarActorFromActorInfo(),
				Start,
				Start + Direction * 75.f,
				1,
				FLinearColor::Red,
				120,
				1);
		}
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SpawnLocation, SpawnLocation + ForwardVector * 100.f, 1, FLinearColor::White, 120, 1);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SpawnLocation, SpawnLocation + LeftVector * 100.f, 1, FLinearColor::Gray, 120, 1);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SpawnLocation, SpawnLocation + RightVector * 100.f, 1, FLinearColor::Gray, 120, 1);
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
			// ManaCost
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
