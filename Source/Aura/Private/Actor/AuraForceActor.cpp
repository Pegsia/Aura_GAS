// Copyright QiuXuan


#include "Actor/AuraForceActor.h"

#include "Aura/Aura.h"
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
	ForceComponent->ForceStrength = -400000.f;
	ForceComponent->Radius = 700.f;
	ForceComponent->Falloff = RIF_Constant;
}


void AAuraForceActor::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraForceActor::OnBeginOverlap);	
}

void AAuraForceActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}