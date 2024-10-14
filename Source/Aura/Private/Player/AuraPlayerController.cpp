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
#include "NiagaraFunctionLibrary.h"
#include "AuraMagicCircleActor.h"
#include "Aura/Aura.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set InputContext
	check(AuraContext);	
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(AuraContext, 0);
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
	SetMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if(!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AAuraMagicCircleActor>(MagicCircleClass);
		bShowMouseCursor = false;
		if(DecalMaterial)
		{
			MagicCircle->SetMaticCircleMaterial(DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if(IsValid(MagicCircle))
	{
		bShowMouseCursor = true;
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::SetMagicCircleLocation()
{
	if(IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.Location);
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{	
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
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
	if(GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if(LastEnemyActor) { LastEnemyActor->UnHighLightActor(); LastEnemyActor = nullptr;}
		if(ThisEnemyActor) { ThisEnemyActor->UnHighLightActor(); ThisEnemyActor = nullptr;}
		return;
	}

	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	if (!GetHitResultUnderCursor(TraceChannel, false, CursorHit)) return;

	// IEnemyInterface
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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased); // 根据绑定的函数选择特定的delegate
	AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;
	
	// 点击时判断是否正在攻击敌人
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisEnemyActor ? true : false;
		bAutoRunning = false;
	}

	GetAuraASC()->AbilityInputPressed(InputTag);	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased)) return;
	
	// Not Moving
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		GetAuraASC()->AbilityInputReleased(InputTag);
		return;
	}

	GetAuraASC()->AbilityInputReleased(InputTag);

	if (!bTargeting && !bShiftDown) // Not Targeting 
	{
		// Auto Running
		if(GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;
		
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{			
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			
			// Need a NavMesh
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

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag) // Tick
{
	if(GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) return;
	
	// Not Moving
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		GetAuraASC()->AbilityInputHeld(InputTag);
		return;
	}

	if (bTargeting || bShiftDown) // Targeting || ShiftDown
	{
		GetAuraASC()->AbilityInputHeld(InputTag);
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
	if(GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) return;
	
	bAutoRunning = false;
	const FVector2D InputActionVector = InputActionValue.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // RotationMatrix第一行，即X轴方向
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);   // RotationMatrix第二行，即Y轴方向

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
	return AuraAbilitySystemComponent;
}