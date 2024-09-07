// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature, int32 /*StateValue*/);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// Player State
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	
	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);

	FOnPlayerStateChangedSignature OnXpChangeDelegate;
	FOnPlayerStateChangedSignature OnLevelChangeDelegate;
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_Level")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_XP")
	int32 XP = 0;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);
};
