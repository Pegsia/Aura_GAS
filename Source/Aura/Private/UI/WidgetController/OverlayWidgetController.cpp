// Copyright QiuXuan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraAttributeSet.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraPlayerState.h"
#include "LevelUpInfo.h"
#include "AuraGameplayTags.h"
#include "AbilityInfo.h"

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

	// Bind Equip Ability
	GetAuraASC()->AbilityEquipSignature.AddUObject(this, &UOverlayWidgetController::OnAbilityEquip);
	
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
	// GetAuraASC()->AbilityCommittedCallbacks.AddLambda(
	// 	[this](const UGameplayAbility* Ability)
	// 	{
	// 		OnAbilityCommittedDelegate.Broadcast(*Ability->GetCooldownTags(), Ability->GetCooldownTimeRemaining());
	// 	});	

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

void UOverlayWidgetController::OnAbilityEquip(const FGameplayTag& AbilityTag, const FGameplayTag& StateTag,	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	// Old Ability Info
	FAuraAbilityInfo OldInfo;
	OldInfo.InputTag = PreviousSlot;
	OldInfo.AbilityStatusTag = AuraTags.Abilities_Status_Locked;
	OldInfo.AbilityTag = AuraTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(OldInfo);
	
	// New Ability Info
	FAuraAbilityInfo NewInfo = AbilityInfo->FindAbilityInfoByAbilityTag(AbilityTag);
	NewInfo.InputTag = Slot;
	NewInfo.AbilityStatusTag = StateTag;
	AbilityInfoDelegate.Broadcast(NewInfo);
}
