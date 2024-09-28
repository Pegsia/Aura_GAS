// Copyright QiuXuan


#include "AbilitySystem/Ability/AuraGameplayAbility_Passive.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemComponent.h"

void UAuraGameplayAbility_Passive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AuraASC->DeactivatePassiveAbilityDelegate.AddUObject(this, &UAuraGameplayAbility_Passive::ReceivedDeactivate);
	}
}

void UAuraGameplayAbility_Passive::ReceivedDeactivate(const FGameplayTag& AbilityTag)
{
	if(AbilityTags.HasTagExact(AbilityTag))
	{
		InternalEndAbility();
	}
}
