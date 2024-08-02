// Copyright QiuXuan


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
	UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
}
