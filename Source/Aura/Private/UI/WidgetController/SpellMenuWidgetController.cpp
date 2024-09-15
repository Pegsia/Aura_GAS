// Copyright QiuXuan


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilityInfo.h"
#include "AuraAbilitySystemComponent.h"

void USpellMenuWidgetController::BindCallBacksToDependencies()
{
	BindAbilityInfo();
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag){
			if(AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByAbilityTag(AbilityTag);
				Info.AbilityStatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
}
