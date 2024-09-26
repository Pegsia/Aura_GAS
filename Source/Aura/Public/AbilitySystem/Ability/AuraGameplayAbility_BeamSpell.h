// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility_Damage.h"
#include "AuraGameplayAbility_BeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_BeamSpell : public UAuraGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	
	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerProperties();

	// Sphere trace to find first hit target 
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTarget);

	UFUNCTION(BlueprintCallable)
	void GetAdditionalTargets(TArray<AActor*>& AdditionalTargets);
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	FScalableFloat AdditionalRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 AdditionalTargetNumber = 5;
};
