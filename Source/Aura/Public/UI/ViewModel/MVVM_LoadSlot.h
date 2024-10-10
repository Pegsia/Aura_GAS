// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AuraSaveGame_LoadSlot.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndexSignature, int32, SwitcherIndex);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndexSignature SetWidgetSwitcherIndexDelegate;

	void InitializeSlot() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString PlayerName;

	void SetPlayerName(FString InPlayerName);
	FString GetPlayerName() const;

	/*Save Game*/
	UPROPERTY()
	FString SaveGame_SlotName;

	UPROPERTY()
	int32 SaveGame_SlotIndex;

	UPROPERTY()
	TEnumAsByte<ESaveGameSlotStatus> SaveGame_SlotStatus;
};
