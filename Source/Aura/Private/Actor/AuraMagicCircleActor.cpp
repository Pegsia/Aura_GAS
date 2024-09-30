// Copyright QiuXuan


#include "Actor/AuraMagicCircleActor.h"
#include "Components/DecalComponent.h"

AAuraMagicCircleActor::AAuraMagicCircleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

void AAuraMagicCircleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

