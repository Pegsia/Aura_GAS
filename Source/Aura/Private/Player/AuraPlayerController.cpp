// Copyright QiuXuan


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set InputContext
	if (ensureAlways(AuraContext))
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		check(SubSystem);
		SubSystem->AddMappingContext(AuraContext, 0);
	}
	// End Set InputContext

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

	CursorTrace();
}


void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// IEnmyInterface
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();
	/**
	 * Line trace from cursor. There are several scenarios.
	 *  A. LastActor is null && ThisActor is null
	 *		- Do nothing
	 *  B. LastActor is null && ThisActor is valid
	 *		- ThisActor->HighLightActor();
	 *	C. LastActor is valid && ThisActor is null
	 *		- LastActor->UnHighLightActor();
	 *	D. Both Actors are valid && LastActor != ThisActor
	 *		- LastActor->UnHighLightActor(); ThisActor->HighLightActor();
	 *	E. Both Actors are valid && LastActor == ThisActor
	 *		- Do nothing
	 */
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr) // Case B
		{
			ThisActor->HighLightActor();
		}
	}
	else // LastActor != nullptr
	{
		if (ThisActor == nullptr) // Case C
		{
			LastActor->UnHighLightActor();
		}
		else
		{
			if (LastActor != ThisActor) // Case D
			{
				LastActor->UnHighLightActor(); 
				ThisActor->HighLightActor();
			}
		}
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
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
