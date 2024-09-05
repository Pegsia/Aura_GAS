// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTagsSignature, FGameplayTagContainer& /* Asset Tags*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityGivenSignature, UAuraAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbilitySignature, const FGameplayAbilitySpec&);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	FEffectAssetTagsSignature EffectAssetTagsDelegate; // Pop up Message
	FAbilityGivenSignature AbilityGivenDelegate;	   // Spells Globe
	
	void AbilityActorInfoSet();
	
	// Init Ability
	void AddCharacterStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	bool bStartupAbilitiesGiven = false;

	// loop through Activatable Abilities, Broadcast Ability Tag and Input Tag
	// FForEachAbilitySignature ForEachAbilityDelegate;	
	void ForEachAbility(const FForEachAbilitySignature& Delegate);

	// Utility Functions
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);
	
	// Input Ability
	void AbilityInputHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

protected:

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	
};
