// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterClassInfo.h"
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
	
	void SaveGame_LoadSlot(const UMVVM_LoadSlot* LoadSlot);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USaveGame> SaveGameLoadSlotClass;
};
