// Copyright QiuXuan


#include "Game/AuraGameModeBase.h"

#include "AuraGameInstance.h"
#include "AuraSaveGame_LoadSlot.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	MapNameToMap.Emplace(DefaultMapName, DefaultMap);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if(!Actors.IsEmpty())
	{
		AActor* SelectedActor = Actors[0];
		for(AActor* Actor : Actors)
		{
			if(APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if(PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = Actor;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void AAuraGameModeBase::SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const
{
	if(!IsValid(LoadSlot)) return;
	
	DeleteSaveGame_LoadSlot(LoadSlot);
	UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::CreateSaveGameObject(SaveGameLoadSlotClass));
	SaveGame_LoadSlot->MapName = LoadSlot->GetMapName();
	SaveGame_LoadSlot->PlayerName = LoadSlot->GetPlayerName();
	SaveGame_LoadSlot->SlotStatus = LoadSlot->SaveGame_SlotStatus;
	SaveGame_LoadSlot->PlayerStartTag = LoadSlot->SaveGame_PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveGame_LoadSlot, LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
}

UAuraSaveGame_LoadSlot* AAuraGameModeBase::LoadSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const
{
	if(!IsValid(LoadSlot)) return nullptr;
	
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
	if(!IsValid(LoadSlot)) return;
	
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
	}
}

void AAuraGameModeBase::LoadMap(const UMVVM_LoadSlot* LoadSlot) const
{
	if(!IsValid(LoadSlot)) return;
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MapNameToMap.FindChecked(LoadSlot->GetMapName()));
}
