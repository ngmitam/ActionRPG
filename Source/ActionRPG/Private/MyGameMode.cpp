// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameMode.h"

#include "MyCharacter.h"
#include "LoadingScreenWidget.h"
#include "PauseMenuWidget.h"
#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AMyCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Create loading screen widget
	if(LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(
			GetWorld(), LoadingScreenWidgetClass);
		if(LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(100); // High Z-order
			LoadingScreenWidget->ShowLoadingScreen(
				FText::FromString(TEXT("Starting Game...")));
		}
	}

	// Create pause menu widget
	if(PauseMenuWidgetClass)
	{
		PauseMenuWidget =
			CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuWidgetClass);
		if(PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport(50); // Medium Z-order
			PauseMenuWidget->HidePauseMenu();
		}
	}

	// Hide loading screen after a short delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this,
		&AMyGameMode::OnLevelLoaded,
		2.0f, // 2 seconds
		false);
}

void AMyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clean up widgets
	if(LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
	}

	if(PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
}

void AMyGameMode::ShowLoadingScreen(const FText &LoadingText)
{
	if(LoadingScreenWidget)
	{
		LoadingScreenWidget->ShowLoadingScreen(LoadingText);
	}
}

void AMyGameMode::HideLoadingScreen()
{
	if(LoadingScreenWidget)
	{
		LoadingScreenWidget->HideLoadingScreen();
	}
}

void AMyGameMode::ShowPauseMenu()
{
	if(PauseMenuWidget)
	{
		PauseMenuWidget->ShowPauseMenu();
	}
}

void AMyGameMode::HidePauseMenu()
{
	if(PauseMenuWidget)
	{
		PauseMenuWidget->HidePauseMenu();
	}
}

void AMyGameMode::TogglePauseMenu()
{
	if(PauseMenuWidget
		&& PauseMenuWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void AMyGameMode::LoadLevel(const FName &LevelName, const FText &LoadingText)
{
	ShowLoadingScreen(LoadingText);

	// Load the level
	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true);
}

void AMyGameMode::OnLevelLoaded()
{
	HideLoadingScreen();
}
