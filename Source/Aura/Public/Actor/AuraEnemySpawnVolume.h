// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "SaveGameInterface.h"
#include "GameFramework/Actor.h"
#include "AuraEnemySpawnVolume.generated.h"

class UBoxComponent;
class AAuraEnemySpawnPoint;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemySpawnVolume();

	/** Begin ISaveGameInterface Interface */
	virtual void LoadActor_Implementation() override;
	/** End  ISaveGameInterface Interface */
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
};
