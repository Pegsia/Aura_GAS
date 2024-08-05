// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "EnemyInterface.h"
#include "AuraEnemy.generated.h"

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	//~ Begin IEnemyInterface
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	//~ End IEnemyInterface

protected:
	//~ Begin AuraCharacterBase Interface
	virtual void InitAbilityActorInfo() override;
	//~ End AuraCharacterBase Interface

	virtual void BeginPlay() override;
};
