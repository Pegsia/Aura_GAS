// Copyright QiuXuan


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::OnEffectApplied);

	const FAuraGameplayTags& AuraGameplayTags= FAuraGameplayTags::Get();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
		FString::Printf(TEXT("Tag: %s"), *AuraGameplayTags.Attribute_Secondary_Armor.ToString()));
}

void UAuraAbilitySystemComponent::OnEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTagContainer;
	EffectSpec.GetAllAssetTags(AssetTagContainer);

	EffectAssetTagsDelegate.Broadcast(AssetTagContainer);
}
