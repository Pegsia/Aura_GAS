// Copyright QiuXuan


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AuraPlayerState.h"
#include "Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallBacksToDependencies()
{ 
	// GetAttributeMenuWidgetController 时绑定
	check(AttributeInfo);

	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : GetAuraAS()->TagsToAttributesFuncPtr)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				//Info.AttributeValue = Data.NewValue;
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}

	GetAuraPS()->OnAttributePointsChangeDelegate.AddLambda(
		[this](int32 AttributePoints)
		{
			AttributePointsChangeDelegate.Broadcast(AttributePoints);
		});
}

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	
	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : GetAuraAS()->TagsToAttributesFuncPtr)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AttributePointsChangeDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

// Set Attribute Value And Broadcast to Attribute Menu
void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpdateAttribute(AttributeTag);
}