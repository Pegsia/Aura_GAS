// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CombatInterface.h"
#include "CharacterClassInfo.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	//~ Begin ICombatInterface
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void CharacterDeath() override;
	//~ End ICombatInterface

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleCharacterDeath();
protected:
	virtual void BeginPlay() override;

	// Combat
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	FName WeaponSocketName{"WeaponHandSocket"};

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponTipSocketName;  // Location to spawn projectiles

	//~ Begin ICombatInterface
	virtual FVector GetCombatSocketLocation() override;
	//~ End ICombatInterface
	
	// Initial Ability Actor Info(PC, PS, ASC, AS)
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitialAbilityActorInfo();

	// Init Default Attributes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaluts")
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaluts")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	virtual void InitialDefaultAttributes() const;

	// Init Abilities
	void InitStartupAbilities();

	// Dissolve Effects
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartMeshDissolveTimeLine(UMaterialInstanceDynamic* MaterialInstanceDynamic);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* MaterialInstanceDynamic);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> MeshDissolveMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
