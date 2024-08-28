// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AuraBTTask_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTTask_EnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetActorKS;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
