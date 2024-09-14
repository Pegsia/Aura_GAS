// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	//~ Begin UAuraWidgetController Interface	
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End UAuraWidgetController Interface
	
};
