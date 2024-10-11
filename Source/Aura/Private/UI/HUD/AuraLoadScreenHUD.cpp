// Copyright QiuXuan


#include "UI/HUD/AuraLoadScreenHUD.h"

#include "AuraLoadScreenWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void AAuraLoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlotViewModel();
	
	LoadScreenWidget = CreateWidget<UAuraLoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
	
	GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
	GetOwningPlayerController()->SetShowMouseCursor(true);	
}
