// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"

#include "AuraSaveGame_LoadSlot.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveGameSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSaveGameAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY()
	FGameplayTag AbilityTag;

	UPROPERTY()
	FGameplayTag AbilityInputTag;
	
	UPROPERTY()
	FGameplayTag AbilityStatusTag;

	UPROPERTY()
	FGameplayTag AbilityTypeTag;

	UPROPERTY()
	int32 AbilityLevel;

	FORCEINLINE bool operator==(const FSaveGameAbility& Other) const
	{
		return AbilityTag == Other.AbilityTag;
	}
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

	/** Abilities */
	UPROPERTY()
	TArray<FSaveGameAbility> SaveGameAbilities;
};
