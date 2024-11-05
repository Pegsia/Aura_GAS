// Copyright QiuXuan


#include "Actor/AuraForceActor.h"

#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

AAuraForceActor::AAuraForceActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>("ForceComponent");
	ForceComponent->SetupAttachment(GetRootComponent());
	ForceComponent->bIgnoreOwningActor = true;
	ForceComponent->ForceStrength = -1000000.f;
	ForceComponent->Radius = 300.f;
	ForceComponent->Falloff = RIF_Linear;
	ForceComponent->bAutoActivate = false;
}

void AAuraForceActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(Duration);

	FireForce();
	FTimerHandle ForceTimer;
	FTimerDelegate ForceDelegate = FTimerDelegate::CreateUObject(this, &AAuraForceActor::FireForce);
	GetWorldTimerManager().SetTimer(ForceTimer, ForceDelegate, Period, true);
	
	// SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraForceActor::OnBeginOverlap);	
}

void AAuraForceActor::FireForce() const
{
	ForceComponent->ToggleActive();
	ForceComponent->FireImpulse();
}

void AAuraForceActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// DrawDebugSphere(GetWorld(), OtherActor->GetActorLocation(),30.f, 6, FColor::Blue, false, 3.f);
}
