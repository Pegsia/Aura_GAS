// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AuraBTTask_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
