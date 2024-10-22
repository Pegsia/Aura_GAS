// Copyright QiuXuan


#include "Actor/AuraEnemySpawnPoint.h"

#include "AuraEnemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	ensure(EnemyClass);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->Level = Level;
	Enemy->CharacterClass = CharacterClass;
	
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
