// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

struct FSelectedAbility
{
	FGameplayTag AbilityTag;
	FGameplayTag AbilityStatusTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSelectedButtonInfoSignature, bool, bSpellPointButtonEnable, bool, bEquipButtonEnable, FString, CurrentLevelDescription, FString, NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipButtonSignature, const FGameplayTag&, AbilityTYpe);

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Spell|Point")
	FOnStateChangedSignature SpellPointsChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Spell|Select")
	FSelectedButtonInfoSignature SelectedButtonInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Spell|Equip")
	FWaitForEquipButtonSignature WaitForEquipButtonDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Spell|Equip")
	FWaitForEquipButtonSignature StopWaitingForEquipButtonDelegate;
	
	UFUNCTION(BlueprintCallable)
	void BroadcastSelectedButtonInfoByAbilityTag(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendSpellPoints();

	UFUNCTION(BlueprintCallable)
	void DeselectGlobe();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	
	//~ Begin UAuraWidgetController Interface	
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End UAuraWidgetController Interface

private:
	// Save current ability status
	int32 CurrentSpellPoints;
	FSelectedAbility SelectedAbility;
	void BroadcastSelectedButtonInfo(const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag, const int32 SpellPoints);

	// Equip
	bool bWaitForEquip = false;
	void StopWaitingForEquipButton(const FGameplayTag& AbilityTag);
};
