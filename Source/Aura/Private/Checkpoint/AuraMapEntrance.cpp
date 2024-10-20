// Copyright QiuXuan


#include "Checkpoint/AuraMapEntrance.h"

#include "PlayerInterface.h"
#include "AuraGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

class AAuraGameModeBase;

AAuraMapEntrance::AAuraMapEntrance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bSaveGameAfterReached = true;
	BoxComponent->SetupAttachment(MoveToComponent);
}

void AAuraMapEntrance::LoadActor_Implementation()
{
	// do nothing
}

void AAuraMapEntrance::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerTag);
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
