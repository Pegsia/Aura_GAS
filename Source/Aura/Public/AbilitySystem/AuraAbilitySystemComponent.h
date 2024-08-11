// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTagsDelegate, FGameplayTagContainer& /* Asset Tags*/)

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	FEffectAssetTagsDelegate EffectAssetTagsDelegate;
	
	void AbilityActorInfoSet();
	
	void AddCharacterStatrupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

protected:
	void OnEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	
};
