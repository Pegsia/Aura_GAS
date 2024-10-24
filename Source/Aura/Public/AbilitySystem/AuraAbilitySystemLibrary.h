// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"


class ULootTiersInfo;
class UAuraSaveGame_LoadSlot;
class AAuraHUD;
struct FWidgetControllerParams;
struct FDamageEffectProperties;
class USpellMenuWidgetController;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class UAbilitySystemComponent;
class UGameplayEffect;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Widget Controller */ 
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool GetWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|EffectToSelf")
	static FActiveGameplayEffectHandle ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Abilities")
	static float GetEffectTimeRemainingByCooldownTag(const UAbilitySystemComponent* InASC, const FGameplayTag& CooldownTag);
	
	/** Initial Character Or Get RPG CharacterInfo By Character Info */ 
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static void InitializeCharacterDefaultAttributes(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* ASC);	

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static void InitializeCharacterDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const UAuraSaveGame_LoadSlot* LoadSlot);
	
	/** Start Up Abilities */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Ability")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static void InitializeStartAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const ECharacterClass& CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|XP")
	static int32 GetXPRewardByClassAndLevel(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, const int32 CharacterLevel);	

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectProperties& DamageEffectProps);

	/** Loot */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Loot")
	static ULootTiersInfo* GetLootTiersInfo(const UObject* WorldContextObject);
	
	/** Combat Detection */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappedActors, const TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin, float Radius);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(const FVector& Origin, const int32& MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsFriend(const AActor* FirstActor, const AActor* SecondActor);

	/** Projectiles */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& ForwardVector, const float& Spread, const int32& NumRotators, const FVector& Axis = FVector::UpVector);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlySpacedVectors(const FVector& ForwardVector, const float& Spread, const int32& NumVectors, const FVector& Axis = FVector::UpVector);

	/** Pop Widget */
	static void ShowPopUpWidget(TSubclassOf<UUserWidget> InWidgetClass, UObject* Outer, USceneComponent* AttachComponent, float DurationTime);
	
	/** ------------------------------------------------------------
	 * Aura Gameplay Effect Context Handle
	 * ------------------------------------------------------------*/ 	
	/** Getter */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockBackVector(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool GetIsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	
	/** Setter */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDamage);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDuration);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetKnockBackVector(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackVector);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool& bInIsRadialDamage);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float& InRadialDamageInnerRadius);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const float& InRadialDamageOuterRadius);

	/** FDamageEffectProperties */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffectProps")
	static void SetDamageEffectPropRadialDamage(UPARAM(ref) FDamageEffectProperties& DamageEffectParams, bool InIsRadialDamage, float InnerRadius, float OuterRadius, FVector Origin);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffectProps")
	static void SetDamageEffectPropKnockBack(UPARAM(ref) FDamageEffectProperties& DamageEffectParams, FVector KnockBackDirection, float Magnitude = 0.f, float KnockBackChance = 0.f);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffectProps")
	static void SetDamageEffectPropDeathImpulse(UPARAM(ref) FDamageEffectProperties& DamageEffectParams, FVector ImpulseDirection, float Magnitude = 0.f);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffectProps")
	static void SetDamageEffectPropTargetASC(UPARAM(ref) FDamageEffectProperties& DamageEffectParams, UAbilitySystemComponent* InASC);
};
