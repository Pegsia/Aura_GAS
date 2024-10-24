// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiersInfo.generated.h"

USTRUCT(BlueprintType)
struct FLootItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TSubclassOf<AActor> LootItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0;

	// if true Level = Character Level, false = 1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class AURA_API ULootTiersInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	TArray<FLootItemInfo> GetSpawnLootItemInfos();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TArray<FLootItemInfo> LootItemInfos;
};
