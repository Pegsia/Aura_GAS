// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
class UAuraUserWidget;
class UAuraWidgetController;
struct FWidgetControllerParams;


UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	virtual void PossessedBy(AController* NewController) override;
	
	//~ Begin IEnemyInterface
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//~ End IEnemyInterface

	//~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override { return Level; }
	virtual void CharacterDeath() override;
	//~ End ICombatInterface

	// OverlayWidgetController Signature
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	void InitHealthBar();

	// Effects
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	
	void HitReactTagChanged(const FGameplayTag CallBackTag, int32 TagCount);

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	// When Enemy Dead, Set it LifeSpan
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	float EnemyLifeSpan = 5.f;

	// Behavior
	UPROPERTY(VisibleAnywhere)
	FName HitReactingKey = "HitReacting";

	UPROPERTY(VisibleAnywhere)
	FName RangedAttackerKey = "RangedAttacker";
protected:
	//~ Begin AuraCharacterBase Interface
	virtual void InitialAbilityActorInfo() override;
	//~ End AuraCharacterBase Interface

	void InitEffects();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
