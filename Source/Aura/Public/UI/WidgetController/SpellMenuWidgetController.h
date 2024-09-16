// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellMenuButtonEnableSignature, bool, bSpellPointButtonEnable, bool, bEquipButtonEnable);

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "GAS|PlayerState")
	FOnStateChangedSignature SpellPointsChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|PlayerState")
	FSpellMenuButtonEnableSignature SpellMenuButtonEnableDelegate;

	UFUNCTION(BlueprintCallable)
	void BroadcastSpellMenuButtonEnableByAbilityTag(const FGameplayTag& AbilityTag);
	
	//~ Begin UAuraWidgetController Interface	
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End UAuraWidgetController Interface
	
};
