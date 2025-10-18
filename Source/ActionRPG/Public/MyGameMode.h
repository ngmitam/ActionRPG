// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyAbilityTypes.h"
#include "MyGameConfig.h"
#include "MinimapCaptureActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MinimapWidget.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

UCLASS()
class ACTIONRPG_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMyGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Loading screen functions
	void ShowLoadingScreen(
		const FText &LoadingText = FText::FromString(TEXT("Loading...")));
	void HideLoadingScreen();

	// Pause menu functions
	void ShowPauseMenu();
	void HidePauseMenu();
	void TogglePauseMenu();

	// Level loading functions
	void LoadLevel(const FName &LevelName,
		const FText &LoadingText = FText::FromString(TEXT("Loading...")));
	void OnLevelLoaded();

	// Minimap setup
	void SetupMinimap();

protected:
	// Loading screen widget
	UPROPERTY()
	class ULoadingScreenWidget *LoadingScreenWidget;

	// Pause menu widget
	UPROPERTY()
	class UPauseMenuWidget *PauseMenuWidget;

	// Widget classes to spawn
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<AMinimapCaptureActor> MinimapCaptureActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	UTextureRenderTarget2D *MinimapRenderTarget;

private:
	/**
	 * Setup the minimap capture actor for rendering the minimap
	 */
	void SetupMinimapCaptureActor();

	/**
	 * Setup the timer for minimap initialization to ensure proper timing
	 */
	void SetupMinimapTimer();

	/**
	 * Initialize the loading screen widget and set it up for display
	 */
	void InitializeLoadingScreen();

	/**
	 * Initialize the pause menu widget and hide it initially
	 */
	void InitializePauseMenu();
};
