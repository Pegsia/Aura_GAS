// Copyright QiuXuan


#include "Actor/AuraPointCollection.h"

#include "AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AAuraPointCollection::AAuraPointCollection()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Point_0 = CreateDefaultSubobject<USceneComponent>("Point_0");
	SetRootComponent(Point_0);

	Point_1 = CreateDefaultSubobject<USceneComponent>("Point_1");
	ImmutablePoints.Emplace(Point_1);
	Point_1->SetupAttachment(GetRootComponent());
	
	Point_2 = CreateDefaultSubobject<USceneComponent>("Point_2");
	ImmutablePoints.Emplace(Point_2);
	Point_2->SetupAttachment(GetRootComponent());
	
	Point_3 = CreateDefaultSubobject<USceneComponent>("Point_3");
	ImmutablePoints.Emplace(Point_3);
	Point_3->SetupAttachment(GetRootComponent());
	
	Point_4 = CreateDefaultSubobject<USceneComponent>("Point_4");
	ImmutablePoints.Emplace(Point_4);
	Point_4->SetupAttachment(GetRootComponent());
	
	Point_5 = CreateDefaultSubobject<USceneComponent>("Point_5");
	ImmutablePoints.Emplace(Point_5);
	Point_5->SetupAttachment(GetRootComponent());
	
	Point_6 = CreateDefaultSubobject<USceneComponent>("Point_6");
	ImmutablePoints.Emplace(Point_6);
	Point_6->SetupAttachment(GetRootComponent());
	
	Point_7 = CreateDefaultSubobject<USceneComponent>("Point_7");
	ImmutablePoints.Emplace(Point_7);
	Point_7->SetupAttachment(GetRootComponent());
	
	Point_8 = CreateDefaultSubobject<USceneComponent>("Point_8");
	ImmutablePoints.Emplace(Point_8);
	Point_8->SetupAttachment(GetRootComponent());
	
	Point_9 = CreateDefaultSubobject<USceneComponent>("Point_9");
	ImmutablePoints.Emplace(Point_9);
	Point_9->SetupAttachment(GetRootComponent());
	
	Point_10 = CreateDefaultSubobject<USceneComponent>("Point_10");
	ImmutablePoints.Emplace(Point_10);
	Point_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> AAuraPointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,	float YawOverride)
{
	checkf(ImmutablePoints.Num() + 1 >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds."));

	// Rotate Points By YawOverride and Move it to the Ground
	TArray<USceneComponent*> ArrayCopy;	

	// Move Point_0 above, so we don't GetLivePlayersWithinRadius multiple times
	// Get Trace Actors To Ignore
	TArray<AActor*> TraceActorsToIgnore;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, TraceActorsToIgnore, TArray<AActor*>(), Point_0->GetComponentLocation(), 600.f);

	// Move point to the ground
	FHitResult BasePointHitResult;
	const FVector&  BasePointTraceStart = Point_0->GetComponentLocation() + FVector(0.f, 0.f, 500.f);
	const FVector&  BasePointTraceEnd = Point_0->GetComponentLocation() - FVector(0.f, 0.f, 500.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(TraceActorsToIgnore);
	GetWorld()->LineTraceSingleByProfile(BasePointHitResult, BasePointTraceStart, BasePointTraceEnd, FName("BlockAll"), QueryParams);

	const FVector& BasePointAdjustLocation = FVector(Point_0->GetComponentLocation().X, Point_0->GetComponentLocation().Y, BasePointHitResult.Location.Z);
	Point_0->SetWorldLocation(BasePointAdjustLocation);
	Point_0->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(BasePointHitResult.ImpactNormal));
	ArrayCopy.Emplace(Point_0);
	
	const FVector& BaseLocation = Point_0->GetComponentLocation();
	for(USceneComponent* Point : ImmutablePoints)
	{
		if(ArrayCopy.Num() - 1 >= NumPoints) return ArrayCopy; // We already have one

		// Rotate Points Based By Point_0
		if(YawOverride != 0.f)
		{
			FVector ToPoint = Point->GetComponentLocation() - BaseLocation;
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Point->SetWorldLocation(BaseLocation + ToPoint);
		}

		FHitResult HitResult;
		const FVector& TraceStart = Point->GetComponentLocation() + FVector(0.f, 0.f, 500.f);
		const FVector& TraceEnd = Point->GetComponentLocation() - FVector(0.f, 0.f, 500.f);
		GetWorld()->LineTraceSingleByProfile(HitResult, TraceStart, TraceEnd, FName("BlockAll"), QueryParams);

		const FVector& AdjustLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, HitResult.Location.Z);
		Point->SetWorldLocation(AdjustLocation);
		Point->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		ArrayCopy.Emplace(Point);
	}
	return ArrayCopy;
}

void AAuraPointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}
