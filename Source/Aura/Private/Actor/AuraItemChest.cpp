// Copyright QiuXuan


#include "Actor/AuraItemChest.h"

#include "AuraEffectActor.h"
#include "Net/UnrealNetwork.h"

AAuraItemChest::AAuraItemChest()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BashMesh");
	SetRootComponent(BaseMesh);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(GetRootComponent());

	SetReplicates(true);
}

void AAuraItemChest::BeginPlay()
{
	Super::BeginPlay();

	BaseMesh->SetCustomDepthStencilValue(HighLightColor.GetIntValue());
	LidMesh->SetCustomDepthStencilValue(HighLightColor.GetIntValue());
}

void AAuraItemChest::Interact_Implementation()
{
	if(!bLidOpened)
	{
		bLidOpened = true;
		OpenChest();
		SpawnItems();
	}
}

void AAuraItemChest::OnRep_LidOpened()
{
	OpenChest();
}

void AAuraItemChest::LoadActor_Implementation()
{
	if(bLidOpened)
	{
		OpenChest();
	}	
}

// void AAuraItemChest::OpenChest_Implementation()
void AAuraItemChest::OpenChest()
{
	LidMesh->SetRelativeRotation(FRotator(110.f, 0.f, 0.f));
}

// void AAuraItemChest::SpawnItems_Implementation()
void AAuraItemChest::SpawnItems()
{
	FTransform SpawnTransform;
	FVector SpawnLocation = BaseMesh->GetSocketLocation(SpawnSocketName);
	SpawnTransform.SetLocation(SpawnLocation + GetActorForwardVector() * FMath::FRandRange(30.f, 100.f));
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	for(const TSubclassOf<AActor> ActorClass : ItemsToSpawn)
	{
		AActor* LootItem = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform);
		if(AAuraEffectActor* EffectItem = Cast<AAuraEffectActor>(LootItem))
		{
			EffectItem->ForceNetUpdate();
			EffectItem->bPauseTickMovement = true;
			EffectItem->SetLifeSpan(10.f);
			EffectItem->NetMulticastSpawnTransform();
		}
	}
}

void AAuraItemChest::HighLightActor_Implementation()
{
	if(!bLidOpened)
	{
		BaseMesh->SetRenderCustomDepth(true);
		LidMesh->SetRenderCustomDepth(true);
	}
}

void AAuraItemChest::UnHighLightActor_Implementation()
{
	BaseMesh->SetRenderCustomDepth(false);
	LidMesh->SetRenderCustomDepth(false);
}

void AAuraItemChest::SetMoveToLocation_Implementation(FVector& OutLocation)
{
	// Don't Implement this, we do nothing for ItemChest 
}

void AAuraItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraItemChest, bLidOpened);
}
