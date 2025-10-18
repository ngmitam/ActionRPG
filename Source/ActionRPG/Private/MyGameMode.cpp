// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "LoadingScreenWidget.h"
#include "MinimapCaptureActor.h"
#include "MinimapWidget.h"
#include "MyCharacter.h"
#include "MyGameConfig.h"
#include "MyPlayerController.h"
#include "MyPlayerUI.h"
#include "PauseMenuWidget.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AMyCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetupMinimapCaptureActor();
	SetupMinimapTimer();
	InitializeLoadingScreen();
	InitializePauseMenu();
}

void AMyGameMode::SetupMinimapCaptureActor()
{
	if(MinimapCaptureActorClass)
	{
		AMinimapCaptureActor *CapturedActor =
			GetWorld()->SpawnActor<AMinimapCaptureActor>(
				MinimapCaptureActorClass, FVector(0.0f, 0.0f, 1000.0f),
				FRotator::ZeroRotator);
		if(CapturedActor && MinimapRenderTarget)
		{
			CapturedActor->SceneCaptureComponent->TextureTarget =
				MinimapRenderTarget;
		}
	}
}

void AMyGameMode::SetupMinimapTimer()
{
	FTimerHandle MinimapTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		MinimapTimerHandle, this, &AMyGameMode::SetupMinimap, 0.1f, false);
}

void AMyGameMode::InitializeLoadingScreen()
{
	if(LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(
			GetWorld(), LoadingScreenWidgetClass);
		if(LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(
				FGameConfig::GetDefault().LoadingScreenZOrder);
			LoadingScreenWidget->ShowLoadingScreen(
				FText::FromString(TEXT("Starting Game...")));
		}
	}

	// Hide loading screen after a short delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this,
		&AMyGameMode::OnLevelLoaded,
		FGameConfig::GetDefault().LoadingScreenDelay, // Loading screen delay
		false);
}

void AMyGameMode::InitializePauseMenu()
{
	if(PauseMenuWidgetClass)
	{
		PauseMenuWidget =
			CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuWidgetClass);
		if(PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport(
				FGameConfig::GetDefault().PauseMenuZOrder);
			PauseMenuWidget->HidePauseMenu();
		}
	}
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

void AMyGameMode::SetupMinimap()
{
	// Get Player UI from Character
	AMyCharacter *PlayerCharacter =
		Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UMyPlayerUI *PlayerUI = nullptr;
	UMinimapWidget *MinimapWidget = nullptr;
	if(PlayerCharacter)
	{
		PlayerUI = PlayerCharacter->PlayerUIWidget;
	}

	// Get Minimap Widget and set render target
	if(PlayerUI)
	{
		MinimapWidget = PlayerUI->GetMinimapWidget();
		if(MinimapWidget && MinimapRenderTarget)
		{
			MinimapWidget->SetRenderTarget(MinimapRenderTarget);
		}
	}
}
