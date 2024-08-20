// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnemyInterface.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	//~ Begin APlayerController Interface
	virtual void PlayerTick(float DeltaTime);
	//~ End APlayerController Interface

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter);
protected:
	//~ Begin APlayerController Interface
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	//~ End APlayerController Interface

private:

	// Input Context
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	bool bShiftDown = false;
	void ShiftPressed() { bShiftDown = true; }
	void ShiftReleased() { bShiftDown = false; }

	// Show Enemy Outline
	FHitResult CursorHit;
	void CursorTrace();
	TScriptInterface<IEnemyInterface> LastEnemyActor;
	TScriptInterface<IEnemyInterface> ThisEnemyActor;

	// Input CallBacks
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig; // FAuraInputAction: UInputAcion, FGameplayTag

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetAuraASC();

	// Click To Move
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	float bTargeting = false;

	// Auto Run
	bool bAutoRunning = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 30.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;	

	void AutoRun();

	// Damage
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
