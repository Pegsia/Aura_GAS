// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnBeginOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;

	/** Tick Movement */
	UPROPERTY(EditDefaultsOnly, Category = "TickMovement")
	bool bRotateMovement = false;

	UPROPERTY(EditDefaultsOnly, Category = "TickMovement")
	float RotationRate = 45.f;

	FRotator InitializeRotation;
	
	UPROPERTY(EditDefaultsOnly, Category = "TickMovement")
	bool bSinusoidalMovement = false;

	FVector InitializeLocation;
	
	UPROPERTY(EditDefaultsOnly, Category = "TickMovement")
	float SineAmplitude = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "TickMovement")
	float SinePeriodConstant = 1.f;

	float SinePeriod = 2 * PI;
	
	void TickMovement(float DeltaTime) const;
	
	/** Effect Apply */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	bool bDestroyOnEffectApplication = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	bool bApplyEffectsToEnemies = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apllied Effect")
	float ActorLevel = 1.f;

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	// Instant Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	// Duration Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	// Infinite Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Apllied Effect")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::DoNotRemove;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandleMap;

private:
	float RunningTime = 0.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> EffectMesh;
};
