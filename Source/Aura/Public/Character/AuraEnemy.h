// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "EnemyInterface.h"
#include "HighLightInterface.h"
#include "Aura/Aura.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "LootTiersInfo.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
class UAuraUserWidget;
class UAuraWidgetController;
struct FWidgetControllerParams;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighLightInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	virtual void PossessedBy(AController* NewController) override;
	
	//~ Begin IHighLightInterface
	virtual void HighLightActor_Implementation() override;
	virtual void UnHighLightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutLocation) override;
	//~ End IHighLightInterface
	
	//~ Begin IEnemyInterface
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//~ End IEnemyInterface

	//~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override { return Level; }
	virtual void CharacterDeath(const FVector& ImpulseVector) override;
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
	
	void HitReactTagChanged(const FGameplayTag CallBackTag, int32 TagCount);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 TagCount) override;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	// When Enemy Dead, Set it LifeSpan
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	float EnemyLifeSpan = 5.f;

	// Behavior
	UPROPERTY(VisibleAnywhere)
	FName HitReactingKey = "HitReacting";
	
	UPROPERTY(VisibleAnywhere)
	FName StunnedKey = "Stunned";
	
	UPROPERTY(VisibleAnywhere)
	FName RangedAttackerKey = "RangedAttacker";

	UPROPERTY(EditDefaultsOnly, Category = "Glow")
	TEnumAsByte<ECustomDepthColor> HighLightColor;
	
	// Loot
	FTimerHandle SpawnHandle;
	
	int32 SpawnLootCount = 0;
	
	TArray<FLootItemInfo> LootItemsInfo;
	
	TArray<FRotator> LootRotations;
	
	UFUNCTION(BlueprintCallable)
	void SpawnLootItem();

	void SpawnOneItem();
	
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
