// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
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
	UTexture2D* Image = nullptr;
};

// Delegate Signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommittedSignature, float, TimeRemaining);

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
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FOnAbilityCommittedSignature OnAbilityCommittedDelegate;

	// Level Delegate
	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangeDelegate;
	
	
	//~ Begin UAuraWidgetController Interface
	virtual void BroadcastInitialValue() override;
	virtual void BindCallBacksToDependencies() override;
	//~ End UAuraWidgetController Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Message Widget")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Message Widget")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	// Get Information about all Abilities, look up their Ability Info, Broadcast AbilityInfo to Widget
	void OnInitializeStartupAbilities() const;
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template<typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
