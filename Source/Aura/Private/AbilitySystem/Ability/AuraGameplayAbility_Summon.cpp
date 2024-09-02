// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Summon.h"

#include "AuraCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<bool> CVarDrawSummonLocations(TEXT("aura.DrawSummonLocations"), false, TEXT("DrawSummonLocations"), ECVF_Cheat);

UAuraGameplayAbility_Summon::UAuraGameplayAbility_Summon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

TArray<FVector> UAuraGameplayAbility_Summon::GetSpawnLocations()
{
	if(NumMinion <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Try To Summon InValid Num Of Minions in GA_Summon"));
		return TArray<FVector>();
	}
	
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	const FVector ForwardVector = Avatar->GetActorForwardVector();
	const FVector Location = Avatar->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinion;

	const FVector LeftOfSpread = ForwardVector.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = ForwardVector.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);

	if(CVarDrawSummonLocations.GetValueOnGameThread())
	{
		DebugDrawSummon(Location + LeftOfSpread * MinSpawnDistance, Location + LeftOfSpread * MaxSpawnDistance, FColor::Red, Avatar);
		DebugDrawSummon( Location + RightOfSpread * MinSpawnDistance, Location + RightOfSpread * MaxSpawnDistance, FColor::Blue, Avatar);
	}

	TArray<FVector> SpawnLocations;
	for(int32 i = 0; i < NumMinion; ++i)
	{
		// By adding DeltaSpread / 2.f to each step, the spawn points start a half-spread from the beginning and stop a half-spread from the end.
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i + DeltaSpread / 2.f, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
	
		FHitResult Hit; // Find a Ground Location
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation + FVector(0.f, 0.f, 400.f), SpawnLocation + FVector(0.f, 0.f, -400.f), ECC_Visibility);
		if(Hit.bBlockingHit)
		{
			SpawnLocation = Hit.Location;
		}
		SpawnLocations.Emplace(SpawnLocation);
		
		if(CVarDrawSummonLocations.GetValueOnGameThread())
		{
			DebugDrawSummon(Location + Direction * MinSpawnDistance, Location + Direction * MaxSpawnDistance, FColor::Green, Avatar);
			DrawDebugSphere(GetWorld(), SpawnLocation, 18.f,  12, FColor::Cyan, false, 3.f);
		}
	}	
	return SpawnLocations;
}

AAuraCharacterBase* UAuraGameplayAbility_Summon::GetRandomMinionClassCDO()
{
	if(MinionClasses.IsEmpty()) return nullptr;
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)]->GetDefaultObject<AAuraCharacterBase>();
}

AAuraCharacterBase* UAuraGameplayAbility_Summon::SpawnMinionAtLocation(const FVector& SpawnLocation)
{
	AAuraCharacterBase* MinionClassCDO = GetRandomMinionClassCDO();
	FActorSpawnParameters Parameters;
	Parameters.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	Parameters.Owner = GetAvatarActorFromActorInfo();
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn Minion
	AAuraCharacterBase* Minion = GetWorld()->SpawnActor<AAuraCharacterBase>(
		MinionClassCDO->GetClass(),
		SpawnLocation + FVector(0.f, 0.f, MinionClassCDO->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		UKismetMathLibrary::FindLookAtRotation(GetAvatarActorFromActorInfo()->GetActorLocation(), SpawnLocation),
		Parameters
		);

	return Minion;
}

void UAuraGameplayAbility_Summon::DebugDrawSummon(const FVector& Start, const FVector& End, const FColor& Color, const AActor* Avatar)
{
	UKismetSystemLibrary::DrawDebugArrow(Avatar, Start, End, 4.f, Color, 3.f, 3.f);
	DrawDebugSphere(GetWorld(), Start, 6.f,  6, Color, false, 3.f);
	DrawDebugSphere(GetWorld(), End, 6.f,  6, Color, false, 3.f);
}
