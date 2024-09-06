// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_WaitCooldownChange.generated.h"

struct FGameplayEffectSpec;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoolDownChangedSignature, float, TimeRemaining);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncAction"))
class AURA_API UAsyncAction_WaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCoolDownChangedSignature OnCoolDownBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCoolDownChangedSignature OnCoolDownEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncAction_WaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndAction();
	
protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayTag CoolDownTag;

	void OnCoolDownTagChanged(const FGameplayTag InCooldownTag, int32 TagCount);
	void OnActiveGameplayEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};
