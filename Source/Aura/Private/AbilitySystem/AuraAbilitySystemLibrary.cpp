// Copyright QiuXuan


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AuraHUD.h"
#include "AuraPlayerState.h"
#include "AuraWidgetController.h"
#include "AuraGameModeBase.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "CombatInterface.h"
#include "Aura/Aura.h"
#include "ScalableFloat.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	// Widget Only exist on clients, PC is the local controlled controller
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

FActiveGameplayEffectHandle UAuraAbilitySystemLibrary::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, UAbilitySystemComponent* ASC)
{
	check(ASC);
	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(ASC->GetAvatarActor());
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	return ActiveEffectHandle;
}

// GetCharacterClassInfo From GameMode
UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr)	return nullptr;
	return GameMode->CharacterClassInfo;  // UCharacterClassInfo
}

void UAuraAbilitySystemLibrary::InitializeCharacterDefaultAttributes(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	if(const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))  // TTuple<ECharacterClass, FCharacterClassDefaultInfo>
	{
		const FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
		ApplyEffectToSelf(ClassDefaultInfo.PrimaryAttributes, Level, ASC);
		ApplyEffectToSelf(ClassDefaultInfo.SecondaryAttributes, Level, ASC);
		ApplyEffectToSelf(ClassInfo->VitalAttributes, Level, ASC);
	}	
}

void UAuraAbilitySystemLibrary::InitializeStartAbilities(const UObject* WorldContextObject,	UAbilitySystemComponent* ASC, const ECharacterClass& CharacterClass)
{
	if(const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		for(TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
			ASC->GiveAbility(AbilitySpec);
		}
		const FCharacterClassDefaultInfo& DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
		for(TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartUpAbilities)
		{
			if(ASC->GetAvatarActor()->Implements<UCombatInterface>())
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
				ASC->GiveAbility(AbilitySpec);
			}
		}
	}	
}

int32 UAuraAbilitySystemLibrary::GetXPRewardByClassAndLevel(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, const int32 CharacterLevel)
{
	if(const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		const FCharacterClassDefaultInfo& Info = ClassInfo->GetClassDefaultInfo(CharacterClass);
		return static_cast<int32>(Info.XPReward.GetValueAtLevel(CharacterLevel));
	}
	return 0;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappedActors, const TArray<AActor*> ActorsToIgnore, const FVector& SphereOrigin, float Radius)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	if(UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),FCollisionShape::MakeSphere(Radius), QueryParams);

		for(FOverlapResult& Overlap : Overlaps)
		{
			if(Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor())) // Alive Combat Interface
			{
				OutOverlappedActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}			
		}
	}
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if(FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,	bool bInIsCriticalHit)
{
	if(FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

bool UAuraAbilitySystemLibrary::IsFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothPlayers = FirstActor->ActorHasTag(ACTOR_TAG_PLAYER) && SecondActor->ActorHasTag(ACTOR_TAG_PLAYER);
	const bool bBothEnemies = FirstActor->ActorHasTag(ACTOR_TAG_ENEMY)  && SecondActor->ActorHasTag(ACTOR_TAG_ENEMY);
	return bBothPlayers || bBothEnemies;
}


