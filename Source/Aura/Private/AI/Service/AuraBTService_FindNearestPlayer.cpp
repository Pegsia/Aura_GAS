// Copyright QiuXuan


#include "AI/Service/AuraBTService_FindNearestPlayer.h"

#include "AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UAuraBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAuraAIController* AuraAIController = Cast<AAuraAIController>(OwnerComp.GetAIOwner());
	APawn* AIPawn = AuraAIController->GetPawn();
	
	const FName TargetTag = AIPawn->ActorHasTag(ACTOR_TAG_PLAYER) ? ACTOR_TAG_ENEMY : ACTOR_TAG_PLAYER;
	TArray<AActor*> ActorWithTags;	
	UGameplayStatics::GetAllActorsWithTag(AIPawn, TargetTag, ActorWithTags);
	
	// AActor* SelfActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActor_KS.SelectedKeyName));
	AActor* ClosestActor = nullptr;
	float ClosestDistance = MAX_FLT;
	for(AActor* Actor : ActorWithTags)
	{
		if(IsValid(Actor) && IsValid(AIPawn))
		{
			const float Distance = AIPawn->GetDistanceTo(Actor);
			if(Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
				// GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("Name %s"), *Actor->GetName()));
			}
		}		
	}
	BBComp->SetValueAsObject(TargetActor_KS.SelectedKeyName, ClosestActor);
	BBComp->SetValueAsFloat(DistanceToTarget_KS.SelectedKeyName, ClosestDistance);
}
