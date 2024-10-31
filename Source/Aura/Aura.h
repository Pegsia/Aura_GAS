// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("Aura_Game"), STATGROUP_Aura, STATCAT_Advanced);

// High Light Color
UENUM(BlueprintType)
enum ECustomDepthColor : uint8
{
	None,
	Red = 250,
	Blue = 251,
	Tan = 252	
};

#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_CursorTarget ECollisionChannel::ECC_GameTraceChannel2
#define ECC_ExcludePlayers ECollisionChannel::ECC_GameTraceChannel3

#define ACTOR_TAG_PLAYER "Player"
#define ACTOR_TAG_ENEMY "Enemy"