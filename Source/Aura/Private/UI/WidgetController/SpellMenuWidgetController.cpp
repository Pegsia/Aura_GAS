// Copyright QiuXuan


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilityInfo.h"
#include "AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void USpellMenuWidgetController::BindCallBacksToDependencies()
{
	BindAbilityInfo();

	// When Level Up, update ability status 
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
		{
			if(AbilityTag.MatchesTagExact(SelectedAbility.AbilityTag))
			{
				SelectedAbility.AbilityStatusTag = StatusTag;
				BroadcastSelectedButtonInfo(StatusTag, AbilityTag, CurrentSpellPoints);
			}
			if(AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByAbilityTag(AbilityTag);
				Info.AbilityStatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});
	
	// When Level Up, broadcast spell points
	GetAuraPS()->OnSpellPointsChangeDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangeDelegate.Broadcast(SpellPoints);
			
			CurrentSpellPoints = SpellPoints;
			BroadcastSelectedButtonInfo(SelectedAbility.AbilityStatusTag, SelectedAbility.AbilityStatusTag, CurrentSpellPoints);
		});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
	SpellPointsChangeDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BroadcastSelectedButtonInfoByAbilityTag(const FGameplayTag& AbilityTag)
{
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFormAbilityTag(AbilityTag);	
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	FGameplayTag StatusTag;	
	if(AbilitySpec == nullptr || AbilityTag.MatchesTagExact(GameplayTags.Abilities_None))
	{
		StatusTag = GameplayTags.Abilities_Status_Locked;
	}
	else
	{		
		StatusTag = GetAuraASC()->GetAbilityStatusTagFromSpec(*AbilitySpec);
	}

	// Cache Current Selected Spell Globe Info
	CurrentSpellPoints = SpellPoints;
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = StatusTag;				
	BroadcastSelectedButtonInfo(StatusTag, AbilityTag, SpellPoints);	
}

void USpellMenuWidgetController::SpendSpellPoints()
{
	GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::BroadcastSelectedButtonInfo(const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag, const int32 SpellPoints)
{
	bool bSpellPointEnable = false;
	bool bEquipEnable = false;	
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bSpellPointEnable = true;
		bEquipEnable = true;
	}
	else if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bSpellPointEnable = true;
	}
		
	if(SpellPoints == 0) bSpellPointEnable = false;

	FString CurrentLevelDescription;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, CurrentLevelDescription, NextLevelDescription);
	SelectedButtonInfoDelegate.Broadcast(bSpellPointEnable, bEquipEnable, CurrentLevelDescription, NextLevelDescription);
}
