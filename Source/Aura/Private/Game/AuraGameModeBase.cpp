// Copyright QiuXuan


#include "Game/AuraGameModeBase.h"

#include "AuraSaveGame_LoadSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	MapNameToMap.Emplace(DefaultMapName, DefaultMap);
}

void AAuraGameModeBase::SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const
{
	DeleteSaveGame_LoadSlot(LoadSlot);
	UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::CreateSaveGameObject(SaveGameLoadSlotClass));

	SaveGame_LoadSlot->PlayerName = LoadSlot->GetPlayerName();
	SaveGame_LoadSlot->SlotStatus = LoadSlot->SaveGame_SlotStatus;
	
	UGameplayStatics::SaveGameToSlot(SaveGame_LoadSlot, LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
}

UAuraSaveGame_LoadSlot* AAuraGameModeBase::LoadSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const
{
	UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = nullptr;
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex))
	{
		SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::LoadGameFromSlot(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex));
	}
	else
	{
		SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::CreateSaveGameObject(SaveGameLoadSlotClass));
	}
	return SaveGame_LoadSlot;
}

void AAuraGameModeBase::DeleteSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot)
{
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
	}
}
