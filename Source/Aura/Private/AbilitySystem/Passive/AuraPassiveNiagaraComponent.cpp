// Copyright QiuXuan


#include "AbilitySystem/Passive/AuraPassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "CombatInterface.h"

UAuraPassiveNiagaraComponent::UAuraPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UAuraPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffectDelegate.AddUObject(this, &UAuraPassiveNiagaraComponent::OnPassiveEffectActivate);
		TryActivateWhenLoadIn(AuraASC);
	}
	else if(TScriptInterface<ICombatInterface> CombatInterface = GetOwner())
	{
		CombatInterface->GetOnAscRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* InASC)
		{
			if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
			{
				AuraASC->ActivatePassiveEffectDelegate.AddUObject(this, &UAuraPassiveNiagaraComponent::OnPassiveEffectActivate);
				TryActivateWhenLoadIn(AuraASC);
			}			
		});
	}
}

void UAuraPassiveNiagaraComponent::OnPassiveEffectActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if(AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if(bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UAuraPassiveNiagaraComponent::TryActivateWhenLoadIn(UAuraAbilitySystemComponent* AuraASC)
{
	if(AuraASC->bStartupAbilitiesGiven)
	{
		if(AuraASC->GetAbilityStatusTagFromAbilityTag(PassiveSpellTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
		{
			Activate();
		}
	}
}
