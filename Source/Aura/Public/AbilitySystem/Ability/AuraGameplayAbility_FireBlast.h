// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"
#include "AuraGameplayAbility_FireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_FireBlast : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="FireBlast")
	int32 NumFireBolts = 12;
	
	//~ Begin AuraGameplayAbility Interface
	virtual FString GetCurrentLevelDescription(const int32& AbilityLevel) override;
	virtual FString GetNextLevelDescription(const int32& AbilityLevel) override;
	//~ End AuraGameplayAbility Interface

private:
	UPROPERTY(EditDefaultsOnly, Category="FireBlast")
	TSubclassOf<AAuraFireBall> FireBallClass;
};
