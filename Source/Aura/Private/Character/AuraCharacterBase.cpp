// Copyright QiuXuan


#include "AuraCharacterBase.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	WeaponSocketName = FName("WeaponHandSocket");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

