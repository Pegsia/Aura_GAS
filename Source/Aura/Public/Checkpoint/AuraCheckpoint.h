// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "AuraCheckpoint.generated.h"

class USphereComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart
{
	GENERATED_BODY()

public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleGlowEffect();
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);	
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	
};
