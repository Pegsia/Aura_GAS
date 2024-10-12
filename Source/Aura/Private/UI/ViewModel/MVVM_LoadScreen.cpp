// Copyright QiuXuan


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "AuraGameInstance.h"
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
	UMVVM_LoadSlot* CurrentSlot = IndexToLoadSlotViewModelMap.FindChecked(SlotIndex);
	CurrentSlot->SaveGame_SlotStatus = EnterName;
	CurrentSlot->InitializeSlot();
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 SlotIndex, const FString& PlayerName)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	
	UMVVM_LoadSlot* CurrentSlot = IndexToLoadSlotViewModelMap.FindChecked(SlotIndex);
	CurrentSlot->SetMapName(AuraGameModeBase->DefaultMapName);
	CurrentSlot->SetPlayerName(PlayerName);
	CurrentSlot->SaveGame_SlotStatus = Taken;
	CurrentSlot->SaveGame_PlayerStartTag = AuraGameModeBase->DefaultPlayerStartTag;
	
	AuraGameModeBase->SaveGame_LoadSlot(CurrentSlot);
	CurrentSlot->InitializeSlot();

	// For Player Start
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->PlayerStartTag = AuraGameModeBase->DefaultPlayerStartTag;
	AuraGameInstance->SaveGame_SlotName = CurrentSlot->SaveGame_SlotName;
	AuraGameInstance->SaveGame_SlotIndex = CurrentSlot->SaveGame_SlotIndex;
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 SlotIndex)
{
	BroadcastSlotSelectedDelegate(true);
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		if(Pair.Key == SlotIndex)
		{
			SelectedSlot = IndexToLoadSlotViewModelMap.FindChecked(SlotIndex);
			Pair.Value->SetSelectButtonEnableDelegate.Broadcast(false);
		}
		else
		{
			Pair.Value->SetSelectButtonEnableDelegate.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if(IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSaveGame_LoadSlot(SelectedSlot);
		SelectedSlot->SaveGame_SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->PlayerStartTag = SelectedSlot->SaveGame_PlayerStartTag;
	
	AuraGameModeBase->LoadMap(SelectedSlot);
}

void UMVVM_LoadScreen::LoadData() // Called In AuraLoadScreenHUD::BeginPlay()
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		UMVVM_LoadSlot* CurrentSlot = Pair.Value;
		UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = AuraGameModeBase->LoadSaveGame_LoadSlot(CurrentSlot);

		CurrentSlot->SetPlayerName(SaveGame_LoadSlot->PlayerName);
		CurrentSlot->SetMapName(SaveGame_LoadSlot->MapName);
		CurrentSlot->SaveGame_SlotStatus = SaveGame_LoadSlot->SlotStatus;
		CurrentSlot->SaveGame_PlayerStartTag = SaveGame_LoadSlot->PlayerStartTag;
		CurrentSlot->InitializeSlot();
	}
}

void UMVVM_LoadScreen::BroadcastSlotSelectedDelegate(const bool bEnable)
{
	SlotSelectedDelegate.Broadcast(bEnable); // Enable Play and Delete Button
}

void UMVVM_LoadScreen::SetAllSelectedButtonEnable(const bool bEnable)
{
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		Pair.Value->SetSelectButtonEnableDelegate.Broadcast(bEnable);
	}
}

void UMVVM_LoadScreen::SetAllNewSlotButtonEnable(const bool bEnable)
{
	for(const TTuple<int32, UMVVM_LoadSlot*>& Pair : IndexToLoadSlotViewModelMap)
	{
		Pair.Value->SetNewSlotButtonEnableDelegate.Broadcast(bEnable);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return IndexToLoadSlotViewModelMap.FindChecked(Index);
}
