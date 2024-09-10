// Copyright QiuXuan


#include "UI/WidgetController/AuraWidgetController.h"

#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
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

UAuraAttributeSet* UAuraWidgetController::GetAuraAS() const
{
	return CastChecked<UAuraAttributeSet>(AttributeSet);
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS() const
{
	return CastChecked<AAuraPlayerState>(PlayerState);
}
