// Copyright QiuXuan


#include "Checkpoint/AuraCheckpoint.h"

#include "AuraAbilitySystemLibrary.h"
#include "AuraGameModeBase.h"
#include "PlayerInterface.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
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

	SaveMessageComponent = CreateDefaultSubobject<UWidgetComponent>("SaveMessage");
	SaveMessageComponent->SetupAttachment(CheckpointMesh);
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
		if(IsValid(SaveMessageComponent))
		{
			SaveMessageComponent->DestroyComponent();
		}
	}
}

void AAuraCheckpoint::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			FString MapName = GetWorld()->GetMapName();
			MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
			
			AuraGameModeBase->SaveWorldState(GetWorld(), MapName);
		}
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		HandleGlowEffect();

		CheckpointMesh->SetRenderCustomDepth(false);
		if(IsValid(SaveMessageComponent))
		{
			SaveMessageComponent->DestroyComponent();
		}
		
		if(IsValid(OtherActor) && SaveWidgetClass)
		{
			UAuraAbilitySystemLibrary::ShowPopUpWidget(SaveWidgetClass, OtherActor, CheckpointMesh, 3.f);
		}
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
	if(bReached) return;
	if(UUserWidget* Message = SaveMessageComponent->GetWidget())
	{
		Message->SetVisibility(ESlateVisibility::Visible);
	}
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraCheckpoint::UnHighLightActor_Implementation()
{
	if(bReached) return;
	if(UUserWidget* Message = SaveMessageComponent->GetWidget())
	{
		Message->SetVisibility(ESlateVisibility::Hidden);
	}
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AAuraCheckpoint::SetMoveToLocation_Implementation(FVector& OutLocation)
{
	OutLocation = MoveToComponent->GetComponentLocation();
}