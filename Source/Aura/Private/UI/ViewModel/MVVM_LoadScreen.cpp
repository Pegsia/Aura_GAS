// Copyright QiuXuan


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlotViewModel()
{
	LoadSlotViewModel_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlotViewModel_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlotViewModel_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	
	LoadSlotViewModel_0->SaveGame_SlotName = FString("LoadSlotViewModel_0");
	LoadSlotViewModel_1->SaveGame_SlotName = FString("LoadSlotViewModel_1");
	LoadSlotViewModel_2->SaveGame_SlotName = FString("LoadSlotViewModel_2");

	LoadSlotViewModel_0->SaveGame_SlotIndex = 0;
	LoadSlotViewModel_1->SaveGame_SlotIndex = 1;
	LoadSlotViewModel_2->SaveGame_SlotIndex = 2;
	
	IndexToLoadSlotViewModelMap.Emplace(0, LoadSlotViewModel_0);
	IndexToLoadSlotViewModelMap.Emplace(1, LoadSlotViewModel_1);
	IndexToLoadSlotViewModelMap.Emplace(2, LoadSlotViewModel_2);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 SlotIndex)
{
	IndexToLoadSlotViewModelMap.FindChecked(SlotIndex)->SetWidgetSwitcherIndexDelegate.Broadcast(1);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 SlotIndex, const FString& PlayerName)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	UMVVM_LoadSlot* CurrentSlot = IndexToLoadSlotViewModelMap.FindChecked(SlotIndex);
	CurrentSlot->SetPlayerName(PlayerName);
	CurrentSlot->SaveGame_SlotStatus = Taken;
	
	AuraGameModeBase->SaveGame_LoadSlot(CurrentSlot);
	CurrentSlot->InitializeSlot();
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 SlotIndex)
{
	BroadcastSlotSelectedDelegate(true);
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		if(Pair.Key == SlotIndex)
		{
			Pair.Value->SetSelectButtonEnableDelegate.Broadcast(false);
		}
		else
		{
			Pair.Value->SetSelectButtonEnableDelegate.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::BroadcastSlotSelectedDelegate(const bool bEnable)
{
	SlotSelectedDelegate.Broadcast(bEnable);
}

void UMVVM_LoadScreen::SetAllSelectedButtonEnable(const bool bEnable)
{
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		Pair.Value->SetSelectButtonEnableDelegate.Broadcast(bEnable);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return IndexToLoadSlotViewModelMap.FindChecked(Index);
}

void UMVVM_LoadScreen::LoadData()
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		UMVVM_LoadSlot* CurrentSlot = Pair.Value;
		UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = AuraGameModeBase->LoadSaveGame_LoadSlot(CurrentSlot);

		CurrentSlot->SetPlayerName(SaveGame_LoadSlot->PlayerName);
		CurrentSlot->SaveGame_SlotStatus = SaveGame_LoadSlot->SlotStatus;
		CurrentSlot->InitializeSlot();
	}
}