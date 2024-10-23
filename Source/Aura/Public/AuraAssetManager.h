// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * A singleton UObject that is responsible for loading and unloading PrimaryAssets
 * and maintaining game-specific asset references
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UAuraAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
