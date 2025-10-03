// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
};
