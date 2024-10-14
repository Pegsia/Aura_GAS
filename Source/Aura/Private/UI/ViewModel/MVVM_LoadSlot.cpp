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

void UMVVM_LoadSlot::SetPlayerLevel(int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

int32 UMVVM_LoadSlot::GetPlayerLevel() const
{
	return PlayerLevel;
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

FString UMVVM_LoadSlot::GetMapName() const
{
	return MapName;
}
