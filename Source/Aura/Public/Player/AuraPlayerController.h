// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnemyInterface.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class IHighLightInterface;
class AAuraMagicCircleActor;
class UNiagaraSystem;
class UDamageTextComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

enum class ETargetingStatus :uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	NotTargeting
};

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	//~ Begin APlayerController Interface
	virtual void PlayerTick(float DeltaTime) override;
	//~ End APlayerController Interface

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
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
	
	UPROPERTY()
	TObjectPtr<AActor> LastActor;
	UPROPERTY()
	TObjectPtr<AActor> ThisActor;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;
	
	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);

	// Input CallBacks
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig; // FAuraInputAction: UInputAcion, FGameplayTag

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	// Can still be nullptr if we call this too early
	UAuraAbilitySystemComponent* GetAuraASC();

	// Click To Move
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;	

	// Auto Run
	bool bAutoRunning = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 30.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;	

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;	
	
	void AutoRun();

	// Damage
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// Arcane
	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TSubclassOf<AAuraMagicCircleActor> MagicCircleClass;

	UPROPERTY()
	TObjectPtr<AAuraMagicCircleActor> MagicCircle;

	void SetMagicCircleLocation();
};
