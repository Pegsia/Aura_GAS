// Copyright QiuXuan


#include "AbilitySystem/Debuff/AuraDebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CombatInterface.h"

UAuraDebuffNiagaraComponent::UAuraDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UAuraDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	TScriptInterface<ICombatInterface> CombatInterface = GetOwner();
	if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAuraDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if(CombatInterface)
	{
		CombatInterface->GetOnAscRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAuraDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
	if(CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UAuraDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(!IsValid(GetOwner())) return;
	if(GetOwner()->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(GetOwner())) return;
	
	if(NewCount > 0)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UAuraDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
