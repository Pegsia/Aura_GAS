// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CombatInterface.h"
#include "CharacterClassInfo.h"
#include "AuraCharacterBase.generated.h"

class UAuraDebuffNiagaraComponent;
class UNiagaraSystem;
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
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByMontageTag_Implementation(const FGameplayTag& Tag) override;
	virtual void CharacterDeath(const FVector& ImpulseVector) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void IncrementMinionCount_Implementation(int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegisteredSignature& GetOnAscRegisteredDelegate() override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	//~ End ICombatInterface

	FOnASCRegisteredSignature OnASCRegisteredDelegate;
	FOnDeathSignature OnDeathDelegate;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (TitleProperty = "MontageTag"))
	TArray<FTaggedMontage> AttackMontageArray;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleCharacterDeath(const FVector& ImpulseVector);
	
protected:
	virtual void BeginPlay() override;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	FName WeaponSocketName{"WeaponHandSocket"};

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponTipSocketName;  // Location to Attack

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName LeftHandSocketName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName TailSocketName;
	
	bool bDead = false;
	
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
	void InitAuraStartupAbilities() const;

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

	// Blood Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	// Death
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USoundBase> DeathSound;

	// Minions
	int32 MinionCount = 0;

	// Debuff
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAuraDebuffNiagaraComponent> BurnDebuffNiagaraComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
};
