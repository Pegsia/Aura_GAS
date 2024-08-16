// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

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

	//~ Begin IEnemyInterface
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	//~ End IEnemyInterface

	//~ Begin ICombatInterface
	virtual int32 GetPlayerLevel() override { return Level; }
	//~ End ICombatInterface

	// OverlayWidgetController Signature
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	void InitHealthBar();

protected:
	//~ Begin AuraCharacterBase Interface
	virtual void InitialAbilityActorInfo() override;
	virtual void InitialDefaultAttributes() const;
	//~ End AuraCharacterBase Interface

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaluts")
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaluts")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarComponent;
};
