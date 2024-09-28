// Copyright QiuXuan


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilityInfo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayAbility.h"
#include "AuraGameplayTags.h"
#include "PlayerInterface.h"
#include "Aura/AuraLogChannels.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// Called On Server
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);

}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTagContainer;
	EffectSpec.GetAllAssetTags(AssetTagContainer);

	EffectAssetTagsDelegate.Broadcast(AssetTagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true; // Set if this Delegate Broadcast too early 上下反过来不行
	AbilityGivenDelegate.Broadcast();		
}

void UAuraAbilitySystemComponent::AddCharacterStartupPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

// Received ActivatableAbilities Broadcast AbilityInfo for Client
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilityGivenDelegate.Broadcast();
	}	
}

void UAuraAbilitySystemComponent::ForEachAbility()
{
	ABILITYLIST_SCOPE_LOCK(); // Lock Abilities Array
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(!ForEachAbilityDelegate.ExecuteIfBound(AbilitySpec)) // Broadcast StartupAbilities for Spells Globe
		{
			UE_LOG(LogAura, Error, TEXT("Failed to Execute Delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	if(Spec.Ability)
	{
		for(const FGameplayTag& Tag : Spec.Ability->AbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities")))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for(const FGameplayTag& Tag : Spec.DynamicAbilityTags)
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities.Status")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& Spec)
{
	for(const FGameplayTag& Tag : Spec.DynamicAbilityTags)
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive()) // Active, InvokeReplicatedEvent
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

// UAbilitySystemComponent provides RPC replication for the actual activation of abilities
// 判断该InputTag下有无可以启动的GA, Called by AuraPlayerController
void UAuraAbilitySystemComponent::AbilityInputHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) //Do Not Active it again
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if(IPlayerInterface::Execute_GetAttributePoint(GetAvatarActor()) > 0)
		{
			ServerUpdateAttribute(AttributeTag);
		}
	}
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFormAbilityTag(const FGameplayTag& AbilityTag)
{
	if(!AbilityTag.IsValid()) return nullptr;
	
	ABILITYLIST_SCOPE_LOCK();
	for(FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for(const FGameplayTag& Tag : Spec.Ability->AbilityTags)
		{
			if(Tag.MatchesTagExact(AbilityTag))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for(FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if(Level < Info.LevelRequirement) continue;
		if(!Info.AbilityTag.IsValid()) continue;
		if(GetSpecFormAbilityTag(Info.AbilityTag) == nullptr) // Give Ability for the first time
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.AbilityClass, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec); // Force spec to replicated now
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFormAbilityTag(AbilityTag))
	{
		if(GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag StatusTag = GetAbilityStatusTagFromSpec(*AbilitySpec);
		if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible)) // Become Unlocked Status
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			StatusTag = GameplayTags.Abilities_Status_Unlocked;
		}
		else if(StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1; // Level Up Ability
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,	const FGameplayTag& InputTag)
{
	// Find Current Ability State and Input Tag, Reset State and Input Tag
	// If this **Selected Spell Button** Ability a valid ability
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFormAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		const FGameplayTag& PreviousAbilityInput = GetInputTagFromSpec(*AbilitySpec); 
		const FGameplayTag& PreviousAbilityState = GetAbilityStatusTagFromSpec(*AbilitySpec);
		
		// Is Available For Equip
		if(PreviousAbilityState.MatchesTagExact(AuraTags.Abilities_Status_Equipped) || PreviousAbilityState.MatchesTagExact(AuraTags.Abilities_Status_Unlocked))
		{
			// Clear **Selected Spell Slot**
			ClearSelectedSlot(InputTag);
			// Clear **Selected Spell Button** just in case if we have already equipped it
			ClearAbilitySlot(*AbilitySpec);
			AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
			if(PreviousAbilityState.MatchesTagExact(AuraTags.Abilities_Status_Unlocked))
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(AuraTags.Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(AuraTags.Abilities_Status_Equipped);
			}
			MarkAbilitySpecDirty(*AbilitySpec);
			ClientEquipAbility(AbilityTag, AuraTags.Abilities_Status_Equipped, InputTag, PreviousAbilityInput);
		}
	}
	
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,	const FGameplayTag& StateTag, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipSignature.Broadcast(AbilityTag, StateTag, Slot, PreviousSlot);
}

void UAuraAbilitySystemComponent::ClearSelectedSlot(const FGameplayTag& Slot)
{
	ABILITYLIST_SCOPE_LOCK();
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTag(Slot))
		{
			ClearAbilitySlot(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ClearAbilitySlot(FGameplayAbilitySpec& AbilitySpec)
{
	const FGameplayTag& Slot = GetInputTagFromSpec(AbilitySpec);
	AbilitySpec.DynamicAbilityTags.RemoveTag(Slot);
	MarkAbilitySpecDirty(AbilitySpec);
}

void UAuraAbilitySystemComponent::ServerUpdateAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutCurrentLevelDescription, FString& OutNextLevelDescription)
{
	if(const FGameplayAbilitySpec* AbilitySpec = GetSpecFormAbilityTag(AbilityTag))
	{
		if(UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutCurrentLevelDescription = AuraAbility->GetCurrentLevelDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if(!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutCurrentLevelDescription = FString();
	}
	else
	{
		OutCurrentLevelDescription = UAuraGameplayAbility::GetLockedDescription(Info->FindAbilityInfoByAbilityTag(AbilityTag).LevelRequirement);
	}	
	OutNextLevelDescription = FString();
	return false;
}

