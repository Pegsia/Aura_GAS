// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlotViewModel();

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 SlotIndex, const FString& PlayerName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 SlotIndex);
	
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
};
