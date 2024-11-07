// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "HighLightInterface.h"
#include "InteractionInterface.h"
#include "SaveGameInterface.h"
#include "Aura/Aura.h"
#include "GameFramework/Actor.h"
#include "AuraItemChest.generated.h"

UCLASS()
class AURA_API AAuraItemChest : public AActor, public IInteractionInterface, public IHighLightInterface, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	AAuraItemChest();

	//~ Begin IHighLightInterface
	virtual void HighLightActor_Implementation() override;
	virtual void UnHighLightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutLocation) override;
	//~ End IHighLightInterface

	//~ Begin IInteractionInterface
	virtual void Interact_Implementation() override;
	//~ End IInteractionInterface

	/** Begin ISaveGameInterface Interface */
	virtual bool LoadTransformFromSaveGame_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	/** End  ISaveGameInterface Interface */

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened", BlueprintReadOnly, SaveGame, Category = "Net")
	bool bLidOpened = false;

	UFUNCTION()
	void OnRep_LidOpened();
	
	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	TEnumAsByte<ECustomDepthColor> HighLightColor = Blue;
	
private:
	void OpenChest() const;

	void SpawnItems();
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	FName SpawnSocketName = FName("SpawnLocation");
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TArray<TSubclassOf<AActor>> ItemsToSpawn;
	
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> LidMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> OpenEffectMesh;
};
