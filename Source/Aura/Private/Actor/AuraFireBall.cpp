// Copyright QiuXuan


#include "Actor/AuraFireBall.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}
