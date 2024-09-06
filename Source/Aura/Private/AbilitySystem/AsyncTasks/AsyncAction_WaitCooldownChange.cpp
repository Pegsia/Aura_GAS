// Copyright QiuXuan


#include "AbilitySystem/AsyncTasks/AsyncAction_WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UAsyncAction_WaitCooldownChange* UAsyncAction_WaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	if(!IsValid(InAbilitySystemComponent) || !InCooldownTag.IsValid()) return nullptr;

	UAsyncAction_WaitCooldownChange* WaitCooldownChangeObject = NewObject<UAsyncAction_WaitCooldownChange>();
	WaitCooldownChangeObject->ASC = InAbilitySystemComponent;
	WaitCooldownChangeObject->CoolDownTag = InCooldownTag;

	// Cooldown has ended(Cooldown Tag Removed)
	InAbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
		WaitCooldownChangeObject, &UAsyncAction_WaitCooldownChange::OnCoolDownTagChanged);

	// Cooldown has been applied
	InAbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		WaitCooldownChangeObject, &UAsyncAction_WaitCooldownChange::OnActiveGameplayEffectAdded);
	
	return WaitCooldownChangeObject;
}

void UAsyncAction_WaitCooldownChange::OnCoolDownTagChanged(const FGameplayTag InCooldownTag, int32 TagCount)
{
	if(TagCount == 0)
	{
		OnCoolDownEnd.Broadcast(0.f);
	}
}

void UAsyncAction_WaitCooldownChange::OnActiveGameplayEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer GrantedTags;
	EffectSpec.GetAllGrantedTags(GrantedTags);
	FGameplayTagContainer AssetTags;
	EffectSpec.GetAllAssetTags(AssetTags);	

	if(GrantedTags.HasTagExact(CoolDownTag) || AssetTags.HasTagExact(CoolDownTag))
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CoolDownTag.GetSingleTagContainer());
		// Get All Effects Remaining Time by CooldownTag, in this cast, it should only contain a single float
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(Query);
		float TimeRemaining = FMath::Max(TimesRemaining);
		OnCoolDownBegin.Broadcast(TimeRemaining);
	}
}

void UAsyncAction_WaitCooldownChange::EndAction()
{
	if(!IsValid(ASC)) return;
	ASC->RegisterGameplayTagEvent(CoolDownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	SetReadyToDestroy();
	MarkAsGarbage();
}
