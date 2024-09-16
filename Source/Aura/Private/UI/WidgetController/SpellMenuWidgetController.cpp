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
				bool bSpellPointEnable = false;
				bool bEquipEnable = false;				
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bSpellPointEnable, bEquipEnable);
				SpellMenuButtonEnableDelegate.Broadcast(bSpellPointEnable, bEquipEnable);
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
			bool bSpellPointEnable = false;
			bool bEquipEnable = false;				
			ShouldEnableButtons(SelectedAbility.AbilityStatusTag, CurrentSpellPoints, bSpellPointEnable, bEquipEnable);
			SpellMenuButtonEnableDelegate.Broadcast(bSpellPointEnable, bEquipEnable);
		});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
	SpellPointsChangeDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BroadcastSpellMenuButtonEnableByAbilityTag(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFormAbilityTag(AbilityTag);	
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
	
	CurrentSpellPoints = SpellPoints;
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = StatusTag;		
		
	bool bSpellPointEnable = false;
	bool bEquipEnable = false;				
	ShouldEnableButtons(StatusTag, SpellPoints, bSpellPointEnable, bEquipEnable);
	SpellMenuButtonEnableDelegate.Broadcast(bSpellPointEnable, bEquipEnable);
}

void USpellMenuWidgetController::SpendSpellPoints()
{
	GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, int32 SpellPoints, bool& OutSpellPointEnable, bool& OutEquipEnable)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		OutSpellPointEnable = true;
		OutEquipEnable = true;
	}
	else if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		OutSpellPointEnable = true;
	}
		
	if(SpellPoints == 0) OutSpellPointEnable = false;
}
