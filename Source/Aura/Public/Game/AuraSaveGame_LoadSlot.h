// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AuraSaveGame_LoadSlot.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSaveGame_LoadSlot : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString PlayerName = FString("DefaultName");
};
