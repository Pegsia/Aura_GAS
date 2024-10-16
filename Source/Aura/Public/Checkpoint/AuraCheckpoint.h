// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "SaveGameInterface.h"
#include "GameFramework/PlayerStart.h"
#include "AuraCheckpoint.generated.h"

class USphereComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveGameInterface
{
	GENERATED_BODY()

public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);

	/** Begin ISaveGameInterface Interface */
	virtual bool LoadTransformFromSaveGame_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	/** End  ISaveGameInterface Interface */
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleGlowEffect();
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);	
	
private:
	UPROPERTY(SaveGame)
	bool bReached = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	
};
