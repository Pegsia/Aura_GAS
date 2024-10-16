// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FString SaveGame_SlotName;

	UPROPERTY()
	int32 SaveGame_SlotIndex;

	// Use PlayerLastTransform for Player Start 
	UPROPERTY()
	bool bInitializeGame = true;
	
	UPROPERTY()
	FTransform PlayerLastTransform;
};
