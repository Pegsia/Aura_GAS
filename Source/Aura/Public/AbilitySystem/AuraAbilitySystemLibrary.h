// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"


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

	// Initial Character Or Get RPG CharacterInfo By Character Info 
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static void InitializeCharacterDefaultAttributes(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* ASC);	

	// Abilities
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|Ability")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefault")
	static void InitializeStartAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const ECharacterClass& CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|XP")
	static int32 GetXPRewardByClassAndLevel(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, const int32 CharacterLevel);	

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectProperties& DamageEffectProps);
	
	// Combat Detection
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappedActors, const TArray<AActor*> ActorsToIgnore, const FVector& SphereOrigin, float Radius);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsFriend(const AActor* FirstActor, const AActor* SecondActor);
	
	/* ------------------------------------------------------------
	 * AuraGameplayEffectContextHandle
	 * ------------------------------------------------------------*/ 	
	/* Getter */
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

	/* Setter */
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
	
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);	
};
