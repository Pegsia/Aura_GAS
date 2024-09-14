// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FAuraAttributeInfo;
struct FGameplayTag;
struct FGameplayAttribute;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

/**
 *  For Attribute Menu Widget and It's SubWidgets
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|PlayerState")
	FOnStateChangedSignature AttributePointsChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|PlayerState")
	FOnStateChangedSignature SpellPointsChangeDelegate;

	UFUNCTION(BlueprintCallable)
	void UpdateAttribute(const FGameplayTag& AttributeTag);
	
	//~ Begin UAuraWidgetController Interface	
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End UAuraWidgetController Interface

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const;
};
