// Copyright QiuXuan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilityInfo.h"
#include "AuraAttributeSet.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraPlayerState.h"
#include "LevelUpInfo.h"

void UOverlayWidgetController::BindCallBacksToDependencies()
{
	AAuraPlayerState* AuraPS = GetAuraPS();
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	const UAuraAttributeSet* AuraAttributeSet = GetAuraAS();	

	// Bind Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);

	// Bind Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);	
	
	// Bind StartupAbilities for Spell Globe
	AuraASC->ForEachAbilityDelegate.BindLambda(
			[this](const FGameplayAbilitySpec& AbilitySpec)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByAbilityTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
				Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
				AbilityInfoDelegate.Broadcast(Info);
			});
	
	// Bind AssetTags for Message
	AuraASC->EffectAssetTagsDelegate.AddLambda(
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
	AuraASC->AbilityCommittedCallbacks.AddLambda(
		[this](const UGameplayAbility* Ability)
		{
			OnAbilityCommittedDelegate.Broadcast(Ability->GetCooldownTimeRemaining());
		});	

	// Bind XP Change
	AuraPS->OnXPChangeDelegate.AddLambda(
		[this, AuraPS](int32 InXP)
		{
			const float Percent = AuraPS->LevelUpInfo->FindPercentForXP(InXP);
			OnXPPercentChangedDelegate.Broadcast(Percent);
		});

	AuraPS->OnLevelChangeDelegate.AddLambda(
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
	const UAuraAttributeSet* AuraAttributeSet = GetAuraAS();

	// Init Health
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	// Init Mana
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	// Broadcast StartupAbilities for Spells Globe
	if (GetAuraASC()->bStartupAbilitiesGiven) // if this Delegate is already BroadCasted
	{
		OnInitializeStartupAbilities();
	}
	else
	{
		GetAuraASC()->AbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities() const
{
	// Broadcast AbilityInfo to Widget
	if(!GetAuraASC()->bStartupAbilitiesGiven) return;
	GetAuraASC()->ForEachAbility();
}
