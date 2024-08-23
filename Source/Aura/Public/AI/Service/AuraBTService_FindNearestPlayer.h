// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AuraBTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector TargetActor_KS;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector DistanceToTarget_KS;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
