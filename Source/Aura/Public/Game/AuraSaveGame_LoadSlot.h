// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AuraSaveGame_LoadSlot.generated.h"

UENUM(BlueprintType)
enum ESaveGameSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraSaveGame_LoadSlot : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bInitializingSaveGame = true;
	
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	
	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	TEnumAsByte<ESaveGameSlotStatus> SlotStatus = Vacant;

	UPROPERTY()
	FName PlayerStartTag;

	/** Player */
	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 PlayerXP = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	/** Attributes */
	UPROPERTY()
	int32 Strength = 0;

	UPROPERTY()
	int32 Intelligence = 0;

	UPROPERTY()
	int32 Resilience = 0;

	UPROPERTY()
	int32 Vigor = 0;
};
