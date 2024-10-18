// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighLightInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHighLightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IHighLightInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void HighLightActor();

	UFUNCTION(BlueprintNativeEvent)
	void UnHighLightActor();
};
