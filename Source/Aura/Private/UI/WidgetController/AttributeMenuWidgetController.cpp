// Copyright QiuXuan


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AS->TagsToAttributesFuncPtr)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BindCallBacksToDependencies()
{
	// GetAttributeMenuWidgetController 时绑定
	check(AttributeInfo);
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AS->TagsToAttributesFuncPtr)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				//Info.AttributeValue = Data.NewValue;
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
}

// Set Attribute Value And Broadcast to Attribute Menu
void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
