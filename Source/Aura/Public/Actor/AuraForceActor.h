// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraForceActor.generated.h"

class USphereComponent;
class URadialForceComponent;

UCLASS()
class AURA_API AAuraForceActor : public AActor
{
	GENERATED_BODY()

public:	
	AAuraForceActor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URadialForceComponent> ForceComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	virtual void BeginPlay() override;
};
