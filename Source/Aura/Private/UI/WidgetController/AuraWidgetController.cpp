// Copyright QiuXuan


#include "UI/WidgetController/AuraWidgetController.h"

#include "AuraAbilitySystemComponent.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilityStstemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValue()
{

}

void UAuraWidgetController::BindCallBacksToDependencies()
{

}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC() const
{
	return CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}
