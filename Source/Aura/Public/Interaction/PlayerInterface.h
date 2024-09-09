// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetLevelForXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 PlayerLevel);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 PlayerLevel);

	UFUNCTION(BlueprintNativeEvent)
	void SetAttributePoint(int32 InAttributePoint);

	UFUNCTION(BlueprintNativeEvent)
	void SetSpellPoint(int32 InSpellPoint);

	UFUNCTION(BlueprintNativeEvent)
	void SetPlayerLevel(int32 InPlayerLevel);
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
