// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/AuraCheckpoint.h"
#include "AuraMapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraMapEntrance : public AAuraCheckpoint
{
	GENERATED_BODY()

public:
	AAuraMapEntrance(const FObjectInitializer& ObjectInitializer);
	
	virtual void LoadActor_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Destination")
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere, Category = "Destination")
	FName DestinationPlayerTag;
	
protected:
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
