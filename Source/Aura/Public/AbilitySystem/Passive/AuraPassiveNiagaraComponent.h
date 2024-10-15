// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "AuraPassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UAuraPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UAuraPassiveNiagaraComponent();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;

	void OnPassiveEffectActivate(const FGameplayTag& AbilityTag, bool bActivate);

	void TryActivateWhenLoadIn(UAuraAbilitySystemComponent* AuraASC);
};
