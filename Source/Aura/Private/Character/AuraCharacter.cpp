// Copyright QiuXuan


#include "Character/AuraCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraPlayerState.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"
#include "AuraHUD.h"
#include "AuraPlayerController.h"
#include "AuraAbilitySystemComponent.h"
#include "MotionWarpingComponent.h"

AAuraCharacter::AAuraCharacter()
{
	// Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	// TopDown Movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // 向加速度方向旋转
	GetCharacterMovement()->RotationRate = AuraRotationRate;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = false;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitialAbilityActorInfo();
	// Init Attribute through GameplayEffects in AuraCharacterBase
	InitialDefaultAttributes(); // 可以只在Server端调用，因为所有变量都是Replicated的，而且ASC复制模式为Mixed
	InitStartupAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the Client
	InitialAbilityActorInfo();
}

void AAuraCharacter::InitialAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	// Init ASC, AS
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	//// Init Attribute through GameplayEffects
	//InitialDefaultAttributes(); // 可以只在Server端调用，因为所有变量都是Replicated的，而且ASC复制模式为Mixed

	/**
	 * Init AuraHUD
	 *  - Set UAuraWideget(WBP_Overlay) && UAuraWidgetController(BP_OverlayWidgetController)
	 *  - Bind Call Backs through WidgetController
	 *	- Broadcast Init Attributes
	 */ 
	if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
	{
		if (AAuraHUD* AuraHUD = AuraPlayerController->GetHUD<AAuraHUD>())
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}	
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

