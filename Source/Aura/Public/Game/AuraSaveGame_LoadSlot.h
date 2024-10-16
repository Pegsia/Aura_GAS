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
	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;

	UPROPERTY()
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY()
	FGameplayTag AbilityInputTag = FGameplayTag();
	
	UPROPERTY()
	FGameplayTag AbilityStatusTag = FGameplayTag();

	UPROPERTY()
	FGameplayTag AbilityTypeTag = FGameplayTag();

	UPROPERTY()
	int32 AbilityLevel = 0;

	FORCEINLINE bool operator==(const FSaveGameAbility& Other) const
	{
		return AbilityTag == Other.AbilityTag;
	}
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform ActorTransform = FTransform();

	// Serialized variables from the Actors - only those marked with SaveGame specifier
	UPROPERTY()
	TArray<uint8> Bytes;

	bool operator==(const FSavedActor& Other) const
	{
		return ActorName == Other.ActorName;
	}
};

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;	
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraSaveGame_LoadSlot : public USaveGame
{
	GENERATED_BODY()

public:
	/** Player Start */
	UPROPERTY()
	bool bInitializingSaveGame = true;
	
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	
	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	TEnumAsByte<ESaveGameSlotStatus> SlotStatus = Vacant;

	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FTransform PlayerLastTransform = FTransform();
	
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

	/** World State */
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	bool HasMap(const FString& InMapName);
	FSavedMap GetSavedMapByMapName(const FString& InMapName);
	bool ReplaceSavedMapByMapName(const FSavedMap& InSavedMap, const FString& InMapName);
};
