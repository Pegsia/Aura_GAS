// Copyright QiuXuan


#include "AI/Task/AuraBTTask_ElementalistAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAIController.h"
#include "AuraGameplayTags.h"
#include "CombatInterface.h"
#include "EnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UAuraBTTask_ElementalistAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAuraAIController* AuraAIController = Cast<AAuraAIController>(OwnerComp.GetAIOwner());
	APawn* AIPawn = AuraAIController->GetPawn();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);
	
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKS.SelectedKeyName));
	if(!TargetActor) return EBTNodeResult::Failed;		
	
	if(!AIPawn->Implements<UEnemyInterface>() || !AIPawn->Implements<UCombatInterface>()) return EBTNodeResult::Failed;
	IEnemyInterface::Execute_SetCombatTarget(AIPawn, TargetActor);

	// Get Minion Count, Chose to Spawn Minions or Attack
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	int32 MinionCount = ICombatInterface::Execute_GetMinionCount(AIPawn);
	FGameplayTag AttackTag = MinionCount < MinionSpawnThreshold ? GameplayTags.Abilities_Summon : GameplayTags.Abilities_Attack;
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(AttackTag);	
	if(!ASC->TryActivateAbilitiesByTag(TagContainer)) return EBTNodeResult::Failed;
	
	return EBTNodeResult::Succeeded;
}
