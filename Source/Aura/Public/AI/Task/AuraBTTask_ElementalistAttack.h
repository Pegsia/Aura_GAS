// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AuraBTTask_ElementalistAttack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTTask_ElementalistAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetActorKS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinionSpawnThreshold = 3;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
