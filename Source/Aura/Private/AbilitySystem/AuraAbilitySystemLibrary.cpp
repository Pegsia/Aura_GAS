// Copyright QiuXuan


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AuraHUD.h"
#include "AuraPlayerState.h"
#include "AuraWidgetController.h"
#include "AuraGameModeBase.h"
#include "AuraAbilitySystemComponent.h"

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

void UAuraAbilitySystemLibrary::InitializeCharacterDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	if (AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		AActor* Avator = ASC->GetAvatarActor();		
		const UCharacterClassInfo* ClassInfo = GameMode->CharacterClassInfo;
		const FCharacterClassDefalutInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

		FGameplayEffectContextHandle PrimaryContext = ASC->MakeEffectContext();
		PrimaryContext.AddSourceObject(Avator);
		const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContext);
		ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());

		FGameplayEffectContextHandle SecondaryContext = ASC->MakeEffectContext();
		SecondaryContext.AddSourceObject(Avator);
		const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, SecondaryContext);
		ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

		FGameplayEffectContextHandle VitalContext = ASC->MakeEffectContext();
		VitalContext.AddSourceObject(Avator);
		const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalContext);
		ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
	}
}
