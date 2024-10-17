// Copyright QiuXuan


#include "Game/AuraGameModeBase.h"

#include "AuraGameInstance.h"
#include "AuraSaveGame_LoadSlot.h"
#include "EngineUtils.h"
#include "SaveGameInterface.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	MapNameToMap.Emplace(DefaultMapName, DefaultMap);
}

// Save, Load, Delete AuraSaveGame
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

	return LoadSaveGame_LoadSlot(LoadSlot->SaveGame_SlotName, LoadSlot->SaveGame_SlotIndex);
}

UAuraSaveGame_LoadSlot* AAuraGameModeBase::LoadSaveGame_LoadSlot(const FString& SlotName, const int32 SlotIndex) const
{
	UAuraSaveGame_LoadSlot* SaveGame_LoadSlot = nullptr;
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGame_LoadSlot = Cast<UAuraSaveGame_LoadSlot>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
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

// Save, Load Current AuraSaveGame
void AAuraGameModeBase::SaveInGameProgressData(UAuraSaveGame_LoadSlot* LoadSlot) const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	AuraGameInstance->PlayerStartTag = LoadSlot->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(LoadSlot, AuraGameInstance->SaveGame_SlotName, AuraGameInstance->SaveGame_SlotIndex);
}

UAuraSaveGame_LoadSlot* AAuraGameModeBase::LoadInGameProgressData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	return LoadSaveGame_LoadSlot(AuraGameInstance->SaveGame_SlotName, AuraGameInstance->SaveGame_SlotIndex);	
}

// Save Actors
void AAuraGameModeBase::SaveWorldState(const UWorld* World) const
{
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	if(UAuraSaveGame_LoadSlot* AuraSaveGame = LoadInGameProgressData())
	{
		// Create Current SavedMap
		// Save SavedMap's Saved Actors
		// Replace SavedMaps with Current SavedMap
		// Save Game to Slot
		if(!AuraSaveGame->HasMap(MapName))
		{
			FSavedMap SavedMap;
			SavedMap.MapName = MapName;
			AuraSaveGame->SavedMaps.Emplace(SavedMap);
		}

		FSavedMap SavedMap = AuraSaveGame->GetSavedMapByMapName(MapName);
		SavedMap.SavedActors.Empty();
		
		for(FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if(!IsValid(Actor) || !Actor->Implements<USaveGameInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.ActorTransform = Actor->GetTransform();

			// Save Actor's Info
			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true; 
			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		if(AuraSaveGame->ReplaceSavedMapByMapName(SavedMap, MapName))
		{
			SaveInGameProgressData(AuraSaveGame);
		}		
	}
}

void AAuraGameModeBase::LoadWorldState(const UWorld* World) const
{
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	if(UAuraSaveGame_LoadSlot* AuraSaveGame = LoadInGameProgressData())
	{
		FSavedMap SavedMap = AuraSaveGame->GetSavedMapByMapName(MapName);
		for(FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->Implements<USaveGameInterface>()) continue;
			
			for(const FSavedActor& SavedActor : SavedMap.SavedActors)
			{
				if(SavedActor.ActorName == Actor->GetFName())
				{
					if(ISaveGameInterface::Execute_LoadTransformFromSaveGame(Actor))
					{
						Actor->SetActorTransform(SavedActor.ActorTransform);
					}
					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);

					ISaveGameInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	// If We Use Player Last Transform
	if(!AuraGameInstance->bInitializeGame && bUsePlayerLastTransform)
	{
		APlayerStart* PlayerStart = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), AuraGameInstance->PlayerLastTransform.GetLocation(), FRotator::ZeroRotator);
		return PlayerStart;
	}
	
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

void AAuraGameModeBase::LoadMap(const UMVVM_LoadSlot* LoadSlot) const
{
	if(!IsValid(LoadSlot)) return;
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MapNameToMap.FindChecked(LoadSlot->GetMapName()));
}
