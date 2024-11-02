// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class USpellMenuWidgetController;
class UAuraUserWidget;
class UAuraWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class UAbilitySystemComponent;
class UAttributeSet;
class UAttributeMenuWidgetController;

UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	void CharacterDeadStart();
	void CharacterDeadFinished();
	
private:
	// Overlay Widget 
	UPROPERTY(EditAnywhere, Category = "AuraHUD")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere, Category = "AuraHUD")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	// Dead Widget
	UPROPERTY(EditAnywhere, Category = "AuraHUD")
	TSubclassOf<UUserWidget> DeadWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> DeadWidget;
	
	// Attribute Menu Widget Controller
	UPROPERTY(EditDefaultsOnly, Category = "AuraHUD")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	// Spell Menu Widget Controller
	UPROPERTY(EditDefaultsOnly, Category = "AuraHUD")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;
};
