// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
struct FGameplayTagContainer;
class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText MessageText = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Image = nullptr;
};

// Delegate Signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuButtonEnableSignature, bool, bEnable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityCommittedSignature, const FGameplayTagContainer&, CooldownTags, float, TimeRemaining);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// Health Delegate
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	// Mana Delegate
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	// Message Widget Row Delegate
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	// Ability Spell Delegate
	UPROPERTY(BlueprintAssignable, Category="GAS|Ability")
	FOnAbilityCommittedSignature OnAbilityCommittedDelegate;

	// Level Delegate
	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnLevelChangedSignature OnLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="UI")
	FOnMenuButtonEnableSignature OnMenuButtonEnableDelegate;

	void SetMenuButtonEnable(bool bEnable) const;
	
	//~ Begin UAuraWidgetController Interface	
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End UAuraWidgetController Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Message Widget")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	// Equip Ability 
	void OnAbilityEquip(const FGameplayTag& AbilityTag,	const FGameplayTag& StateTag, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const;
};

template<typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
