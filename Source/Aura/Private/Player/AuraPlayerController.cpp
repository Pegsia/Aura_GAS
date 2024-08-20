// Copyright QiuXuan


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set InputContext
	if (ensureAlways(AuraContext))
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (SubSystem)
		{
			SubSystem->AddMappingContext(AuraContext, 0);
		}		
	}
	
	// Set Default
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	AutoRun();
	CursorTrace();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter)
{
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(DamageAmount);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		// 根据spline中最近点的方向移动
		const FVector AutoRunLocation = SplineComponent->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector AutoRunDirection = SplineComponent->FindDirectionClosestToWorldLocation(AutoRunLocation, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(AutoRunDirection);

		const float Distance = (AutoRunLocation - CachedDestination).Length();
		if (Distance <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (!GetHitResultUnderCursor(ECC_Visibility, false, CursorHit)) return;

	// IEnmyInterface
	LastEnemyActor = ThisEnemyActor;
	ThisEnemyActor = CursorHit.GetActor();

	if (LastEnemyActor != ThisEnemyActor) // Changed
	{
		if(LastEnemyActor) LastEnemyActor->UnHighLightActor();
		if(ThisEnemyActor) ThisEnemyActor->HighLightActor();
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 点击时判断是否正在攻击敌人
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisEnemyActor ? true : false;
		bAutoRunning = false;
	}

	if (!GetAuraASC()) return;	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// Not Moving
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputReleased(InputTag);
		return;
	}

	if (GetAuraASC()) GetAuraASC()->AbilityInputReleased(InputTag);

	if (!bTargeting && !bShiftDown) // Not Targeting 
	{
		// Auto Running
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				SplineComponent->ClearSplinePoints();
				for (const FVector& PointLocation : NavPath->PathPoints)
				{
					SplineComponent->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(), PointLocation, 16.f, 8, FColor::Cyan, false, 3.f);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints.Last(); // 将终点设置为path最后的点，确保该点可达
				}
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag) // tick
{
	// Not Moving
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputHeld(InputTag);
		return;
	}

	if (bTargeting || bShiftDown) // Targeting || ShiftDown
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputHeld(InputTag);
	}
	else           
	{
		// Continue Moving
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
			if (APawn* ControlledPawn = GetPawn())
			{
				const FVector MoveDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
				const float Distance = (CachedDestination - ControlledPawn->GetActorLocation()).Length();
				if (Distance <= AutoRunAcceptanceRadius) return;

				ControlledPawn->AddMovementInput(MoveDirection);
			}
		}
	}
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	bAutoRunning = false;
	const FVector2D InputActionVector = InputActionValue.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 平行，无roll和pith
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputActionVector.Y); // W和S在FVector2D（二维）中的Y轴
		ControlledPawn->AddMovementInput(RightDirection, InputActionVector.X);
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent; // Can still be nullptr if we call this too early
}