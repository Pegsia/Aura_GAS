// Copyright QiuXuan


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AuraPlayerState.h"
#include "Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallBacksToDependencies()
{ 
	// GetAttributeMenuWidgetController 时绑定
	check(AttributeInfo);
	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AuraAS->TagsToAttributesFuncPtr)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				//Info.AttributeValue = Data.NewValue;
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}

	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPS->OnAttributePointsChangeDelegate.AddLambda(
		[this](int32 AttributePoints)
		{
			AttributePointsChangeDelegate.Broadcast(AttributePoints);
		});
	AuraPS->OnSpellPointsChangeDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangeDelegate.Broadcast(SpellPoints);
		});
}

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);

	for (TTuple<FGameplayTag, FGameplayAttribute(*)()>& Pair : AuraAS->TagsToAttributesFuncPtr)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(PlayerState);
	AttributePointsChangeDelegate.Broadcast(AuraPS->GetAttributePoints());
	SpellPointsChangeDelegate.Broadcast(AuraPS->GetSpellPoints());
}

// Set Attribute Value And Broadcast to Attribute Menu
void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
