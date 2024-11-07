// Copyright QiuXuan


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AuraPlayerController.h"
#include "CombatInterface.h"
#include "PlayerInterface.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	// Used in AttributeMenuWidgetController to Broadcast Attributes
	// Primary Attributes
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Vital_Mana, GetManaAttribute);
	
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	// Secondary Attributes
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

	// Resistance Attributes
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributesFuncPtr.Emplace(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// Change Current Value
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	// Level Up Recalculate MaxHealth and MaxMana
	if(bRestoreHealth && Attribute == GetMaxHealthAttribute())
	{
		bRestoreHealth = false;
		SetHealth(GetMaxHealth());
	}
	if(bRestoreMana && Attribute == GetMaxManaAttribute())
	{
		bRestoreMana = false;
		SetMana(GetMaxMana());
	}
}

bool UAuraAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	if(!IsValid(Data.Target.AbilityActorInfo->AvatarActor.Get()))
	{
		return false;
	}

	return true;
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// I am Dead
	if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;
	
	// Change Base Value
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f Health Changed on %s, Health: %f"), Data.EvaluatedData.Magnitude, *Props.TargetAvatarActor->GetName(), GetHealth()));
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%f Mana Changed on %s, Mana: %f"), Data.EvaluatedData.Magnitude, *Props.TargetAvatarActor->GetName(), GetMana()));
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())  // SERVER, IncomingDamage is NOT REPLICATED
	{
		HandleIncomingDamage(Props);
	}	
	if(Data.EvaluatedData.Attribute == GetIncomingXPAttribute()) // SERVER, IncomingXP is NOT REPLICATED
	{
		HandleIncomingXP(Props);
	}
	if(Data.EvaluatedData.Attribute == GetIncomingSpellPointAttribute())
	{
		HandleIncomingSpellPoint(Props);
	}
	if(Data.EvaluatedData.Attribute == GetIncomingAttributePointAttribute())
	{
		HandleIncomingAttributePoint(Props);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Damage Taken: %f"), LocalIncomingDamage));
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0, GetMaxHealth()));
			
		const bool bFatal = (NewHealth <= 0.f);
		if(!bFatal)
		{
			// HitReact
			if(Props.TargetAvatarActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_GetIsShocked(Props.TargetAvatarActor))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}			

			// KnockBack
			const FVector& KnockBackVector = UAuraAbilitySystemLibrary::GetKnockBackVector(Props.EffectContextHandle);
			if(!KnockBackVector.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockBackVector, true, true);
			}
		}
		else // Death
		{
			if(TScriptInterface<ICombatInterface> CombatInterface = Props.TargetAvatarActor)
			{
				CombatInterface->CharacterDeath(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			if(Props.TargetCharacter->Implements<UPlayerInterface>()) return;
			SendXPEvent(Props);
		}

		// Passive
		HandlePassiveEffect(Props, LocalIncomingDamage);

		// UI
		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingDamage(Props, LocalIncomingDamage, bBlockedHit, bCriticalHit);

		// Debuff
		if(UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			HandleDebuff(Props);
		}
	}
}

void UAuraAttributeSet::HandlePassiveEffect(const FEffectProperties& Props, const float& LocalIncomingDamage)
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	for(const TTuple<FGameplayTag, FGameplayTag>& Pair : AuraTags.PassiveAbilityToAttributes)
	{		
		const FGameplayTag& PassiveAbilityTag = Pair.Key;
		const FGameplayTag& AttributesTag = Pair.Value;
		if(!Props.SourceASC->HasMatchingGameplayTag(PassiveAbilityTag)) continue;

		FGameplayEffectContextHandle PassiveContextHandle = Props.SourceASC->MakeEffectContext();
		PassiveContextHandle.AddSourceObject(Props.SourceAvatarActor);
		
		const FString PassiveEffectName = FString::Printf(TEXT("PassiveEffect_%s"), *PassiveAbilityTag.ToString());
		UGameplayEffect* PassiveEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(PassiveEffectName));

		PassiveEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
		
		FGameplayModifierInfo PassiveModInfo;
		PassiveModInfo.ModifierMagnitude = FScalableFloat(LocalIncomingDamage * 0.2);
		PassiveModInfo.ModifierOp = EGameplayModOp::Additive;
		const TStaticFuncPtr<FGameplayAttribute()> AttributeFuncPtr = *TagsToAttributesFuncPtr.Find(AttributesTag);
		PassiveModInfo.Attribute = AttributeFuncPtr();
		int32 PassiveEffectIndex = PassiveEffect->Modifiers.Emplace(PassiveModInfo);

		if(const FGameplayEffectSpec* PassiveSpec = new FGameplayEffectSpec(PassiveEffect, PassiveContextHandle, 1.f))
		{
			Props.SourceASC->ApplyGameplayEffectSpecToSelf(*PassiveSpec);
		}
	}
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	// Make Gameplay Effect, Set Effect Props
	// Make Mutable Effect Spec
	FGameplayEffectContextHandle DebuffContextHandle = Props.SourceASC->MakeEffectContext();
	DebuffContextHandle.AddSourceObject(Props.SourceAvatarActor);

	// Get Props from Source EffectContextHandle. i.e. the props set in AuraGameplayAbility_Damage
	const FGameplayTag& DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);	
	const float& DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	const float& DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float& DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);

	// Create DebuffEffect
	const FString DebuffEffectName = FString::Printf(TEXT("DebuffEffect_%s"), *DamageType.ToString());
	UGameplayEffect* DebuffEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffEffectName));

	// Set Policy
	DebuffEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DebuffEffect->Period = FScalableFloat(DebuffFrequency);
	DebuffEffect->DurationMagnitude = FScalableFloat(DebuffDuration);

	// Set Modifier
	FGameplayModifierInfo DebuffModInfo;
	DebuffModInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	DebuffModInfo.ModifierOp = EGameplayModOp::Additive;
	DebuffModInfo.Attribute = GetIncomingDamageAttribute();
	int32 DebuffEffectIndex = DebuffEffect->Modifiers.Emplace(DebuffModInfo);

	// Miscellaneous
	DebuffEffect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
	DebuffEffect->StackLimitCount = 1;

	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	const FGameplayTag& DebuffTag = AuraTags.DamageTypesToDebuffs[DamageType];
	DebuffEffect->InheritableOwnedTagsContainer.AddTag(DebuffTag); // Give the GE Debuff Tag to trigger ASC->RegisterGameplayTagEvent
	if(DebuffTag.MatchesTagExact(AuraTags.Debuff_Stun)) // Block Input
	{
		DebuffEffect->InheritableOwnedTagsContainer.AddTag(AuraTags.Player_Block_CursorTrace);
		DebuffEffect->InheritableOwnedTagsContainer.AddTag(AuraTags.Player_Block_InputHeld);
		DebuffEffect->InheritableOwnedTagsContainer.AddTag(AuraTags.Player_Block_InputPressed);
		DebuffEffect->InheritableOwnedTagsContainer.AddTag(AuraTags.Player_Block_InputReleased);
	}

	if(const FGameplayEffectSpec* DebuffSpec = new FGameplayEffectSpec(DebuffEffect, DebuffContextHandle, 1.f))
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(DebuffSpec->GetContext().Get());
		const TSharedPtr<FGameplayTag> SharedDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(SharedDamageType);
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*DebuffSpec);
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	// SourceCharacter is the Owner of this AS, GA_ListenForEvents triggers GE_EventBasedEffect, which applies to self
	if(Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAuraAttributeSet::HandleIncomingSpellPoint(const FEffectProperties& Props)
{
	const float LocalIncomingSpellPoint = GetIncomingSpellPoint();
	SetIncomingSpellPoint(0.f);
	if(Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, LocalIncomingSpellPoint);
	}
}

void UAuraAttributeSet::HandleIncomingAttributePoint(const FEffectProperties& Props)
{
	const float LocalIncomingAttributePoint = GetIncomingAttributePoint();
	SetIncomingAttributePoint(0.f);
	if(Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, LocalIncomingAttributePoint);
	}
}

void UAuraAttributeSet::RefillVitalAttributes()
{
	bRestoreHealth = true;
	bRestoreMana = true;
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if(Props.TargetCharacter->Implements<UCombatInterface>())
	{				
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardByClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}

void UAuraAttributeSet::ShowFloatingDamage(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit)
{
	// if(Props.SourceCharacter != Props.TargetCharacter) {}

	if(AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceController)) // Target is Enemy
	{
		PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
	}
	else if(AAuraPlayerController* PlayerPC = Cast<AAuraPlayerController>(Props.TargetController)) // Target is Player
	{
		PlayerPC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutProps) const
{
	// Source: Causer of the effect, Target = Target of the effect(Owner of this AttributeSet)
	// Source
	OutProps.EffectContextHandle = Data.EffectSpec.GetEffectContext();	
	OutProps.SourceASC = OutProps.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(OutProps.SourceASC) && OutProps.SourceASC->AbilityActorInfo.IsValid())
	{
		OutProps.SourceAvatarActor = OutProps.SourceASC->GetAvatarActor();
		OutProps.SourceController = OutProps.SourceASC->AbilityActorInfo->PlayerController.Get();
		OutProps.SourceCharacter = Cast<ACharacter>(OutProps.SourceAvatarActor);
		if (OutProps.SourceController == nullptr && OutProps.SourceAvatarActor != nullptr)
		{
			OutProps.SourceController = OutProps.SourceCharacter->GetController();
		}
	}

	// Target
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		OutProps.TargetASC = &Data.Target;
		OutProps.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		OutProps.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		OutProps.TargetCharacter = Cast<ACharacter>(OutProps.TargetAvatarActor);
	}
}

/* ---------------------------------------------------------------
	FGameplayAttributeData Corresponding Boiler Plate
-----------------------------------------------------------------*/
/**
 * Vital  Attributes
 */ 
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

/**
 * Primary  Attributes
 */
void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

/**
 * Secondary Attributes
 */
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	/**
	 * Vital  Attributes
	 */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);	

	/**
	 * Primary  Attributes
	 */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/**
	 * Secondary Attributes
	 */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/**
	 * Damage Resistance
	 */
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}
