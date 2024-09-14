// Copyright QiuXuan


#include "UI/WidgetController/SpellMenuWidgetController.h"

void USpellMenuWidgetController::BindCallBacksToDependencies()
{
	BindAbilityInfo();
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
}
