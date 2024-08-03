// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// Health Delegate
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	// Mana Delegate
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxManaChanged;

	//~ Begin UAuraWidgetController Interface
	virtual void BroadcastInitialValue() override;
	virtual void BindCallBacksToDependencies() override;
	//~ End UAuraWidgetController Interface

protected:
	// Health CallBacks
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;

	// Mana CallBacks
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
};
