// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterClassInfo.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "AuraGameModeBase.generated.h"

class UMVVM_LoadSlot;
class USaveGame;
class UAbilityInfo;

/**
 *  AuraGameModeBase
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Default RPG Info
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Default")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Default")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	// Save, Load, Delete AuraSaveGame, Init SaveGameSlot
	void SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const;

	UAuraSaveGame_LoadSlot* LoadSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const;
	UAuraSaveGame_LoadSlot* LoadSaveGame_LoadSlot(const FString& SlotName, const int32 SlotIndex) const;
	
	static void DeleteSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot);
	
	// Save, Load Current AuraSaveGame, Save and Load SaveGameSlot
	void SaveInGameProgressData(UAuraSaveGame_LoadSlot* LoadSlot) const;
	
	UAuraSaveGame_LoadSlot* LoadInGameProgressData() const;

	// Save Actors
	void SaveWorldState(const UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(const UWorld* World) const;

	// Load selected slot Map
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TMap<FString, TSoftObjectPtr<UWorld>> MapNameToMap;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	
	void LoadMap(const UMVVM_LoadSlot* LoadSlot) const;

	// Play Start
	UPROPERTY(EditDefaultsOnly, Category = "Player Start")
	bool bUsePlayerLastTransform = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Start")
	FName DefaultPlayerStartTag;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USaveGame> SaveGameLoadSlotClass;
};
