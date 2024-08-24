// Copyright QiuXuan


#include "AI/Task/AuraBTTask_MeleeAttack.h"

#include "AuraAIController.h"

EBTNodeResult::Type UAuraBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	
	AAuraAIController* AuraAIController = Cast<AAuraAIController>(OwnerComp.GetAIOwner());
	APawn* AIPawn = AuraAIController->GetPawn();
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("Name %s"), *AIPawn->GetName()));
	
	return EBTNodeResult::Succeeded;
}
