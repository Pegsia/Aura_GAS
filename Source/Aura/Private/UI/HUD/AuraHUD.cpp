// Copyright QiuXuan


#include "UI/HUD/AuraHUD.h"
#include "AuraUserWidget.h"
#include "OverlayWidgetController.h"
#include "AttributeMenuWidgetController.h"
#include "SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallBacksToDependencies(); //尽快绑定，不管有没有人订阅
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr) // only one
	{		
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallBacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if(SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		SpellMenuWidgetController->BindCallBacksToDependencies();
	}
	return SpellMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Over Widget Class Uninitalized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class Uninitalized, please fill out BP_AuraHUD"));
	checkf(AttributeMenuWidgetControllerClass, TEXT("AttributeMenu Widget Controller Class Uninitalized, please fill out BP_AuraHUD"));
	
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	// Set OverlayWidgetController
	const FWidgetControllerParams WCParams(PC, PS, ASC, AS);
	UOverlayWidgetController* OverlayWC = GetOverlayWidgetController(WCParams);
	OverlayWidget->SetWidgetController(OverlayWC);
	
	// Broadcast Init Attribute
	OverlayWC->BroadcastInitialValue();

	OverlayWidget->AddToViewport();
}

void AAuraHUD::CharacterDeadStart()
{
	if(DeadWidgetClass)
	{
		DeadWidget = CreateWidget<UUserWidget>(GetWorld(), DeadWidgetClass);
		DeadWidget->AddToViewport();
	}
	OverlayWidget->RemoveFromParent();
}

void AAuraHUD::CharacterDeadFinished()
{
	if(DeadWidget)
	{
		DeadWidget->RemoveFromParent();
	}
}
