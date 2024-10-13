// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "PlayerInterface.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class AAuraPlayerController;
class UCameraComponent;
class USpringArmComponent;
class UMotionWarpingComponent;
class UNiagaraComponent;
class AAuraPlayerState;

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
	virtual int32 GetAttributePoint_Implementation() override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetSpellPoint_Implementation() override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& PlayerStartTag) override;
	//~ End IPlayerInterface
	
	//~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	//~ End ICombatInterface

protected:
	virtual void OnRep_Burned() override;
	virtual void OnRep_Stunned() override;
	
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

	void LoadProgress();
	
	AAuraPlayerState* GetAuraPSChecked() const;
	AAuraPlayerController* GetAuraPCChecked() const;
};
