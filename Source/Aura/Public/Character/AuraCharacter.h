// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "PlayerInterface.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UMotionWarpingComponent;
class UNiagaraComponent;

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	//~ End APawn Interface

	//~ Begin IPlayerInterface
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetLevelForXP_Implementation(int32 InXP) override;
	virtual int32 GetAttributePointReward_Implementation(int32 PlayerLevel) override;
	virtual int32 GetSpellPointReward_Implementation(int32 PlayerLevel) override;
	virtual void SetAttributePoint_Implementation(int32 InAttributePoint) override;
	virtual void SetSpellPoint_Implementation(int32 InSpellPoint) override;
	virtual void SetPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void LevelUp_Implementation(int32 NumberOfLevelUps) override;
	//~ End IPlayerInterface
	
	//~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	//~ End ICombatInterface

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	
	UPROPERTY(EditAnywhere, Category = "Component")
	FRotator AuraRotationRate{0.f, 400.f, 0.f};

private:
	//~ Begin AuraCharacterBase Interface
	virtual void InitialAbilityActorInfo() override;
	//~ End AuraCharacterBase Interface

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastLevelUpVFX() const;
};
