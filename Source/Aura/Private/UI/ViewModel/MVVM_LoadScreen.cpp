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
	AuraGameModeBase->SaveGame_LoadSlot(CurrentSlot);
	CurrentSlot->InitializeSlot();
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 SlotIndex)
{
	
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return IndexToLoadSlotViewModelMap.FindChecked(Index);
}
