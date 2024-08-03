// Copyright QiuXuan


#include "UI/HUD/AuraHUD.h"
#include "AuraUserWidget.h"
#include "OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallBacksToDependencies(); //尽快绑定，不管有没有人订阅

		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Over Widget Class Uninitalized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class Uninitalized, please fill out BP_AuraHUD"));
	
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	// Set OverlayWidgetController
	const FWidgetControllerParams WCParams(PC, PS, ASC, AS);
	UOverlayWidgetController* OverlayWC = GetOverlayWidgetController(WCParams);
	OverlayWidget->SetWidgetController(OverlayWC);
	// Init Attribute
	OverlayWC->BroadcastInitialValue();

	OverlayWidget->AddToViewport();
}
