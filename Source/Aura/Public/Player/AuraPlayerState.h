// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAuraAttributeSet;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;
class UAuraAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature, int32 /*StateValue*/);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// Level Up System
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUpInfo")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	
	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);
	void SetAttributePoints(int32 InAttributePoints);
	void SetSpellPoints(int32 InSpellPoints);
	
	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InAttributePoints);
	void AddToSpellPoints(int32 InSpellPoints);
	
	FOnPlayerStateChangedSignature OnXPChangeDelegate;
	FOnPlayerStateChangedSignature OnLevelChangeDelegate;
	FOnPlayerStateChangedSignature OnAttributePointsChangeDelegate;
	FOnPlayerStateChangedSignature OnSpellPointsChangeDelegate;

	UAuraAbilitySystemComponent* GetAuraASC() const;
	UAuraAttributeSet* GetAuraAS() const;
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;	

private:
	// Level Up System
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_Level")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_XP")
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_AttributePoints")
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = "OnRep_SpellPoints")
	int32 SpellPoints = 0;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributesPoints);
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
