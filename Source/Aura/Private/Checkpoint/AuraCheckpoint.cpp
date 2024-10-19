// Copyright QiuXuan


#include "Checkpoint/AuraCheckpoint.h"

#include "AuraGameModeBase.h"
#include "PlayerInterface.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraCheckpoint::AAuraCheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(CheckpointMesh); // Follow the CheckpointMesh
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());

	NativeGlowTimeLine = CreateDefaultSubobject<UTimelineComponent>("NativeGlowTimeLine");
}

void AAuraCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if(bSaveGameAfterReached)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnBoxOverlap);
	}
	else
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnBoxOverlapWithoutSave);
	}
	
	CheckpointMesh->SetCustomDepthStencilValue(HighLightColor.GetIntValue()); // Already MarkRenderStateDirty
	
	// Glow Timeline
	FOnTimelineFloatStatic GlowTimelineDelegate;
	GlowTimelineDelegate.BindUObject(this, &AAuraCheckpoint::CheckpointReachedNative);
	NativeGlowTimeLine->AddInterpFloat(NativeGlowTrack, GlowTimelineDelegate);
}

void AAuraCheckpoint::LoadActor_Implementation()
{
	if(bReached)
	{
		HandleGlowEffect();
	}
}

void AAuraCheckpoint::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld());
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		HandleGlowEffect();
	}
}

// Just Glow, such as a beacon
void AAuraCheckpoint::OnBoxOverlapWithoutSave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bReached = true;
	HandleGlowEffect();
}

void AAuraCheckpoint::HandleGlowEffect()
{
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlowDynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, GlowDynamicMaterialInstance);
	NativeGlowTimeLine->PlayFromStart();
}

void AAuraCheckpoint::CheckpointReachedNative(const float Glow) const
{
	if(IsValid(GlowDynamicMaterialInstance))
	{
		GlowDynamicMaterialInstance->SetScalarParameterValue(GlowMaterialName, Glow);
	}
}

void AAuraCheckpoint::HighLightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraCheckpoint::UnHighLightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AAuraCheckpoint::SetMoveToLocation_Implementation(FVector& OutLocation)
{
	OutLocation = MoveToComponent->GetComponentLocation();
}