// Copyright QiuXuan


#include "AbilitySystem/Passive/AuraPassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemComponent.h"
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
	}
	else if(TScriptInterface<ICombatInterface> CombatInterface = GetOwner())
	{
		CombatInterface->GetOnAscRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* InASC)
		{
			if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
			{
				AuraASC->ActivatePassiveEffectDelegate.AddUObject(this, &UAuraPassiveNiagaraComponent::OnPassiveEffectActivate);
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
