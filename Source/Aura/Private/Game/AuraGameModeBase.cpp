// Copyright QiuXuan


#include "Game/AuraGameModeBase.h"

#include "AuraSaveGame_LoadSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot)
{
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
	}
	UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::CreateSaveGameObject(SaveGameLoadSlotClass));
	SaveGame_LoadSlot->PlayerName = LoadSlot->PlayerName;
	UGameplayStatics::SaveGameToSlot(SaveGame_LoadSlot, LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
}
