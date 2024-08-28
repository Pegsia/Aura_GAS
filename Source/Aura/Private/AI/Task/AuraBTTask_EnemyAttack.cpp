// Copyright QiuXuan


#include "AI/Task/AuraBTTask_EnemyAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAIController.h"
#include "AuraGameplayTags.h"
#include "EnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

// GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("Name %s"), *AIPawn->GetName()));
EBTNodeResult::Type UAuraBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAuraAIController* AuraAIController = Cast<AAuraAIController>(OwnerComp.GetAIOwner());
	APawn* AIPawn = AuraAIController->GetPawn();
	
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKS.SelectedKeyName));
	if(!TargetActor) return EBTNodeResult::Failed;	

	// Set TargetActor To AuraEnemy In order to MotionWarping, GetTargetActor In GA_MeleeAttack	
	TScriptInterface<IEnemyInterface> EnemyInterface = AIPawn;
	if(!EnemyInterface) return EBTNodeResult::Failed;
	IEnemyInterface::Execute_SetCombatTarget(AIPawn, TargetActor);	

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GameplayTags.Abilities_Attack);
	if(!ASC->TryActivateAbilitiesByTag(TagContainer)) return EBTNodeResult::Failed;
	
	return EBTNodeResult::Succeeded;
}
