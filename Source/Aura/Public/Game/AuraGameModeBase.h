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
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Default")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Default")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	void SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const;

	UAuraSaveGame_LoadSlot* LoadSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot) const;

	static void DeleteSaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot);

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TMap<FString, TSoftObjectPtr<UWorld>> MapNameToMap;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USaveGame> SaveGameLoadSlotClass;
};
