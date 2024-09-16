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
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag){
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
		});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
	SpellPointsChangeDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BroadcastSpellMenuButtonEnableByAbilityTag(const FGameplayTag& AbilityTag)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFormAbilityTag(AbilityTag))
	{
		const FGameplayTag StatusTag =  GetAuraASC()->GetAbilityStatusTagFromSpec(*AbilitySpec);
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
		
		bool bSpellPointEnable = false;
		bool bEquipEnable = false;		
		
		if(StatusTag.MatchesTagExact(Tags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(Tags.Abilities_Status_Unlocked))
		{
			bSpellPointEnable = true;
			bEquipEnable = true;
		}
		else if(StatusTag.MatchesTagExact(Tags.Abilities_Status_Eligible))
		{
			bSpellPointEnable = true;
		}
		
		if(SpellPoints == 0) bSpellPointEnable = false;
		SpellMenuButtonEnableDelegate.Broadcast(bSpellPointEnable, bEquipEnable);
	}
	else
	{
		SpellMenuButtonEnableDelegate.Broadcast(false, false);
	}	
}