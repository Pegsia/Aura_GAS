// Copyright QiuXuan


#include "AI/Task/AuraBTTask_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAIController.h"
#include "AuraGameplayTags.h"

EBTNodeResult::Type UAuraBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	
	AAuraAIController* AuraAIController = Cast<AAuraAIController>(OwnerComp.GetAIOwner());
	APawn* AIPawn = AuraAIController->GetPawn();
	// GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("Name %s"), *AIPawn->GetName()));

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GameplayTags.Abilities_Attack);
	if(!ASC->TryActivateAbilitiesByTag(TagContainer)) return EBTNodeResult::Failed;
	
	return EBTNodeResult::Succeeded;
}
