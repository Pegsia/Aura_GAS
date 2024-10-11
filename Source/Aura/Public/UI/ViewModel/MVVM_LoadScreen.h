// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotSelectedSignature, bool, Selected);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSlotSelectedSignature SlotSelectedDelegate;
	
	void InitializeLoadSlotViewModel();
	void LoadData();
	
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 SlotIndex, const FString& PlayerName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void BroadcastSlotSelectedDelegate(const bool bEnable);
	
	UFUNCTION(BlueprintCallable)
	void SetAllSelectedButtonEnable(const bool bEnable);

	UFUNCTION(BlueprintCallable)
	void SetAllNewSlotButtonEnable(const bool bEnable);
	
	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;
	
private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> IndexToLoadSlotViewModelMap;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_2;

	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;
};
