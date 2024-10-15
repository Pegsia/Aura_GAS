// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraSaveGame_LoadSlot;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTagsSignature, FGameplayTagContainer& /* Asset Tags*/)
DECLARE_MULTICAST_DELEGATE(FAbilityGivenSignature);
DECLARE_DELEGATE_OneParam(FForEachAbilitySignature, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChangedSignature, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /*Status Tag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipSignature, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /*Status Tag*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*Previous Slot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbilitySignature, const FGameplayTag& /*Ability Tag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffectSignature, const FGameplayTag& /*Ability Tag*/, bool /*bActivate*/)

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	FEffectAssetTagsSignature EffectAssetTagsDelegate; // Pop up Message
	FAbilityGivenSignature AbilityGivenDelegate;	   // Spells Globe
	FAbilityStatusChangedSignature AbilityStatusChangedDelegate; // Spell Menu
	FAbilityEquipSignature AbilityEquipSignature; // Spell Menu
	FDeactivatePassiveAbilitySignature DeactivatePassiveAbilityDelegate; // AuraGA_Passive
	FActivatePassiveEffectSignature ActivatePassiveEffectDelegate;
	
	void AbilityActorInfoSet();
	
	// Init Ability
	void AddCharacterLoadedAbilities(const UAuraSaveGame_LoadSlot* SaveGame_LoadSlot);
	
	void AddCharacterStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	bool bStartupAbilitiesGiven = false;

	void AddCharacterStartupPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	
	// loop through Activatable Abilities, Broadcast Ability Tag and Input Tag
	void ForEachAbility(const FForEachAbilitySignature& ForEachAbilityDelegate);

	// Utility Functions
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetAbilityStatusTagFromSpec(const FGameplayAbilitySpec& Spec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& Spec);

	FGameplayTag GetAbilityStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	bool IsPassiveAbility(const FGameplayTag& AbilityTag) const;
	bool AbilityHasEquipped(const FGameplayAbilitySpec& Spec) const;
	
	// Input Ability
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	// Spent Attribute Points and Update Attributes
	void UpdateAttribute(const FGameplayTag& AttributeTag);

	// Ability Update
	// Loop through Activatable Abilities Check if we already have this ability
	FGameplayAbilitySpec* GetSpecFormAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayAbilitySpec* GetSpecFormInputTag(const FGameplayTag& InputTag);
	void UpdateAbilityStatuses(int32 Level);

	// Spell Menu Widget Controller
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

	// Spell Row Button pressed, Equip Ability
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& NewAbilityTag, const FGameplayTag& NewInputTag);

	// Spell Menu Descriptions
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutCurrentLevelDescription, FString& OutNextLevelDescription);
protected:
	// Send Up Ability Info For Spell Globes
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Server, Reliable)
	void ServerUpdateAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, const bool bActivate);
		
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StateTag, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);
	
	void ClearSelectedSlot(const FGameplayTag& Slot);
	void ClearAbilitySlot(FGameplayAbilitySpec& AbilitySpec);
};
