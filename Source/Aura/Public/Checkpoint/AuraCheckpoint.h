// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "HighLightInterface.h"
#include "SaveGameInterface.h"
#include "Aura/Aura.h"
#include "GameFramework/PlayerStart.h"
#include "AuraCheckpoint.generated.h"

class UTimelineComponent;
class USphereComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveGameInterface, public IHighLightInterface
{
	GENERATED_BODY()

public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);

	/** Begin ISaveGameInterface Interface */
	virtual bool LoadTransformFromSaveGame_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	/** End  ISaveGameInterface Interface */

	//~ Begin IHighLightInterface
	virtual void HighLightActor_Implementation() override;
	virtual void UnHighLightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutLocation) override;
	//~ End IHighLightInterface
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxOverlapWithoutSave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void HandleGlowEffect();
	
	void CheckpointReachedNative(const float Glow) const;

	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	TEnumAsByte<ECustomDepthColor> HighLightColor;

	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	bool bSaveGameAfterReached = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	TObjectPtr<UCurveFloat> NativeGlowTrack = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	FName GlowMaterialName = FName("Glow");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	
private:
	UPROPERTY(SaveGame)
	bool bReached = false;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> GlowDynamicMaterialInstance;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> NativeGlowTimeLine;
};
