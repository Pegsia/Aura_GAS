// Copyright QiuXuan


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AuraHUD.h"
#include "AuraPlayerState.h"
#include "AuraWidgetController.h"
#include "AuraGameModeBase.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "CombatInterface.h"
#include "Aura/Aura.h"
#include "ScalableFloat.h"
#include "Components/WidgetComponent.h"

DECLARE_CYCLE_STAT(TEXT("GetPlayersWithinRadius"), STAT_GetPlayersWithinRadius, STATGROUP_Aura);

bool UAuraAbilitySystemLibrary::GetWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	// Widget Only exist on clients, PC is the local controlled controller
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = PC->GetHUD<AAuraHUD>();
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.AttributeSet = AS;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if(GetWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if(GetWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if(GetWidgetControllerParams(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
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

float UAuraAbilitySystemLibrary::GetEffectTimeRemainingByCooldownTag(const UAbilitySystemComponent* InASC, const FGameplayTag& CooldownTag)
{
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
	TArray<float> TimesRemaining = InASC->GetActiveEffectsTimeRemaining(Query);
	return FMath::Max<float>(TimesRemaining);
}

// GetCharacterClassInfo From GameMode
UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)	return nullptr;
	return AuraGameMode->CharacterClassInfo;  // UCharacterClassInfo
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

void UAuraAbilitySystemLibrary::InitializeCharacterDefaultAttributesFromSaveData(const UObject* WorldContextObject,	UAbilitySystemComponent* ASC, const UAuraSaveGame_LoadSlot* LoadSlot)
{
	if(const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))  // TTuple<ECharacterClass, FCharacterClassDefaultInfo>
	{
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(ASC->GetAvatarActor());
		
		const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->PrimaryAttributesSetByCaller, 1.f, EffectContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraGameplayTags.Attributes_Primary_Strength, LoadSlot->Strength);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraGameplayTags.Attributes_Primary_Intelligence, LoadSlot->Intelligence);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraGameplayTags.Attributes_Primary_Resilience, LoadSlot->Resilience);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraGameplayTags.Attributes_Primary_Vigor, LoadSlot->Vigor);
		
		ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
		ApplyEffectToSelf(ClassInfo->GetClassDefaultInfo(ECharacterClass::HeroAura).SecondaryAttributes, 1.f, ASC);
		ApplyEffectToSelf(ClassInfo->VitalAttributes, 1.f, ASC);		
	}	
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->AbilityInfo;
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectProperties& DamageEffectProps)
{
	const FAuraGameplayTags& AuraTag = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectProps.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(DamageEffectProps.SourceASC->GetAvatarActor());
	SetDeathImpulse(EffectContextHandle, DamageEffectProps.DeathImpulse);
	
	const bool bKnockBack = FMath::RandRange(1, 100) < DamageEffectProps.KnockBackChance;
	if(bKnockBack) SetKnockBackVector(EffectContextHandle, DamageEffectProps.KnockBackVector);
	else SetKnockBackVector(EffectContextHandle, FVector::ZeroVector);

	SetIsRadialDamage(EffectContextHandle, DamageEffectProps.bIsRadialDamage);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectProps.RadialDamageOrigin);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectProps.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectProps.RadialDamageOuterRadius);
	
	FGameplayEffectSpecHandle EffectSpecHandle = DamageEffectProps.SourceASC->MakeOutgoingSpec(DamageEffectProps.DamageEffectClass, DamageEffectProps.AbilityLevel, EffectContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, DamageEffectProps.DamageType, DamageEffectProps.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraTag.Debuff_Damage, DamageEffectProps.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraTag.Debuff_Chance, DamageEffectProps.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraTag.Debuff_Frequency, DamageEffectProps.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, AuraTag.Debuff_Duration, DamageEffectProps.DebuffDuration);

	DamageEffectProps.TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	return EffectContextHandle;
}

ULootTiersInfo* UAuraAbilitySystemLibrary::GetLootTiersInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)	return nullptr;
	return AuraGameMode->LootTiersInfo;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappedActors, const TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin, float Radius)
{
	SCOPE_CYCLE_COUNTER(STAT_GetPlayersWithinRadius);
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

void UAuraAbilitySystemLibrary::GetClosestTargets(const FVector& Origin, const int32& MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets)
{
	if(Targets.IsEmpty() || MaxTargets == 0) return;
	
	TArray<AActor*> SortedActors = Targets;
	Algo::Sort(SortedActors, [Origin](const AActor* ActorA, const AActor* ActorB)
	{
		return FVector::DistSquared(ActorA->GetActorLocation(), Origin) < FVector::DistSquared(ActorB->GetActorLocation(), Origin);
	});
	
	for (int32 i = 0; i < FMath::Min(Targets.Num(), MaxTargets); ++i)
	{
		OutClosestTargets.Add(SortedActors[i]);
	}
}

bool UAuraAbilitySystemLibrary::IsFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	const bool bBothPlayers = FirstActor->ActorHasTag(ACTOR_TAG_PLAYER) && SecondActor->ActorHasTag(ACTOR_TAG_PLAYER);
	const bool bBothEnemies = FirstActor->ActorHasTag(ACTOR_TAG_ENEMY)  && SecondActor->ActorHasTag(ACTOR_TAG_ENEMY);
	return bBothPlayers || bBothEnemies;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& ForwardVector, const float Spread,	const int32 NumRotators, const FVector& Axis)
{
	TArray<FRotator> Rotators;
	if(NumRotators <= 1) Rotators.Emplace(ForwardVector.Rotation());
	else
	{
		float MutableSpread = FMath::Min<float>(Spread, 360.f);
		const FVector LeftVector = ForwardVector.RotateAngleAxis(-MutableSpread / 2.f, Axis);
		const float DeltaSpread = MutableSpread == 360.f ? MutableSpread / NumRotators : MutableSpread / (NumRotators - 1);
		
		for(int i = 0; i < NumRotators; ++i)
		{
			const FVector Direction = LeftVector.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Emplace(Direction.Rotation());
		}
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlySpacedVectors(const FVector& ForwardVector, const float Spread, const int32 NumVectors, const FVector& Axis)
{
	TArray<FVector> Vectors;
	if(NumVectors <= 1) Vectors.Emplace(ForwardVector);
	else
	{
		float MutableSpread = FMath::Min<float>(Spread, 360.f);
		const FVector LeftVector = ForwardVector.RotateAngleAxis(-MutableSpread / 2.f, Axis);
		const float DeltaSpread = MutableSpread == 360.f ? MutableSpread / NumVectors : MutableSpread / (NumVectors - 1);
		
		for(int i = 0; i < NumVectors; ++i)
		{
			const FVector Direction = LeftVector.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Emplace(Direction);
		}
	}
	return Vectors;
}

void UAuraAbilitySystemLibrary::ShowPopUpWidget(TSubclassOf<UUserWidget> InWidgetClass, UObject* Outer,	USceneComponent* AttachComponent, float DurationTime)
{
	UWidgetComponent* SaveWidgetComponent = NewObject<UWidgetComponent>(Outer, UWidgetComponent::StaticClass());
	SaveWidgetComponent->RegisterComponent();
	SaveWidgetComponent->AttachToComponent(AttachComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SaveWidgetComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // 马上脱离不会跟着角色移动
			
	SaveWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	SaveWidgetComponent->SetWidgetClass(InWidgetClass);
	FTimerHandle DestroyHandle;
	FTimerDelegate Delegate;
	Delegate.BindLambda([SaveWidgetComponent](){ SaveWidgetComponent->DestroyComponent(); });
	Outer->GetWorld()->GetTimerManager().SetTimer(DestroyHandle, Delegate, DurationTime, false);
}

/* Getter */
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if(AuraEffectContext->GetDamageType().IsValid())
		return *AuraEffectContext->GetDamageType();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockBackVector(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockBackVector();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::GetIsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetIsRadialDamage();
	}
	return false;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

/* Setter */
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraEffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockBackVector(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockBackVector)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockBackVector(InKnockBackVector);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, const bool& bInIsRadialDamage)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, const float& InRadialDamageInnerRadius)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, const float& InRadialDamageOuterRadius)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetDamageEffectPropRadialDamage(FDamageEffectProperties& DamageEffectParams, bool InIsRadialDamage, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = InIsRadialDamage;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetDamageEffectPropKnockBack(FDamageEffectProperties& DamageEffectParams, FVector KnockBackDirection, float Magnitude, float KnockBackChance)
{
	KnockBackDirection.Normalize();
	DamageEffectParams.KnockBackChance = KnockBackChance;
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockBackVector = KnockBackDirection * DamageEffectParams.KnockBackMagnitude;
	}
	else
	{
		DamageEffectParams.KnockBackVector = KnockBackDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDamageEffectPropDeathImpulse(FDamageEffectProperties& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDamageEffectPropTargetASC(FDamageEffectProperties& DamageEffectParams, UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetASC = InASC;
}




