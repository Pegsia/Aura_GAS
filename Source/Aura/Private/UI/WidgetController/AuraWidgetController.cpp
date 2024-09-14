// Copyright QiuXuan


#include "UI/WidgetController/AuraWidgetController.h"


#include "AbilityInfo.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraAttributeSet.h"
#include "AuraPlayerController.h"
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


void UAuraWidgetController::BindAbilityInfo()
{
	GetAuraASC()->ForEachAbilityDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByAbilityTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
			Info.AbilityStatusTag = UAuraAbilitySystemComponent::GetAbilityStatusTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		});
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if(!GetAuraASC()->bStartupAbilitiesGiven) return;
	GetAuraASC()->ForEachAbility();
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if(AuraPlayerController == nullptr)
	{
		AuraPlayerController = CastChecked<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if(AuraPlayerState == nullptr)
	{
		AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if(AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}


