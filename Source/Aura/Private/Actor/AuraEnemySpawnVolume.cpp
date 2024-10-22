// Copyright QiuXuan


#include "Actor/AuraEnemySpawnVolume.h"

#include "AuraEnemySpawnPoint.h"
#include "PlayerInterface.h"
#include "Components/BoxComponent.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECC_WorldStatic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	//编译阶段调用虚函数
	//SetActorHiddenInGame(false);
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(false);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->Implements<UPlayerInterface>()) return;

	bReached = true;
	for(AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if(IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if(bReached)
	{
		Destroy();
	}
}