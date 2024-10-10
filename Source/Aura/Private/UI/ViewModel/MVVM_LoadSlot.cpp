// Copyright QiuXuan


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot() const
{
	const int32 SlotStatus = SaveGame_SlotStatus.GetIntValue();
	SetWidgetSwitcherIndexDelegate.Broadcast(SlotStatus);
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

FString UMVVM_LoadSlot::GetPlayerName() const
{
	return PlayerName;
}
