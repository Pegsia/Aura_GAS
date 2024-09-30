// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraMagicCircleActor.generated.h"

UCLASS()
class AURA_API AAuraMagicCircleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraMagicCircleActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;

};
