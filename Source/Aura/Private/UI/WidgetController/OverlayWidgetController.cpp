// Copyright QiuXuan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraAttributeSet.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraPlayerState.h"
#include "LevelUpInfo.h"

void UOverlayWidgetController::BindCallBacksToDependencies()
{
	// Bind Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);

	// Bind Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);	
	
	// Bind StartupAbilities for Spell Globe
	BindAbilityInfo();
	
	// Bind AssetTags for Message
	GetAuraASC()->EffectAssetTagsDelegate.AddLambda(
		[this](const FGameplayTagContainer& AssetTagContainer)
		{
			for (const FGameplayTag& Tag : AssetTagContainer)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(TEXT("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});

	// Bind Ability Committed for Cooldown time
	GetAuraASC()->AbilityCommittedCallbacks.AddLambda(
		[this](const UGameplayAbility* Ability)
		{
			OnAbilityCommittedDelegate.Broadcast(Ability->GetCooldownTimeRemaining());
		});	

	// Bind XP Change
	GetAuraPS()->OnXPChangeDelegate.AddLambda(
		[this](int32 InXP)
		{
			const float Percent = GetAuraPS()->LevelUpInfo->FindPercentForXP(InXP);
			OnXPPercentChangedDelegate.Broadcast(Percent);
		});

	GetAuraPS()->OnLevelChangeDelegate.AddLambda(
		[this](int32 InLevel)
		{
			OnLevelChangedDelegate.Broadcast(InLevel);
		});
}

/*
 * Called After BindCallBacksToDependencies()
 */
void UOverlayWidgetController::BroadcastInitialValue() 
{
	// Init Health
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());

	// Init Mana
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());

	// StartupAbilities has been given, gather AbilityInfo, Broadcast AbilityInfo to Spell Globes
	if (GetAuraASC()->bStartupAbilitiesGiven) // if this Delegate has already BroadCasted
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraASC()->AbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
}