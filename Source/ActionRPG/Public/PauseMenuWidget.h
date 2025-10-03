// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * Widget for pause menu with quit functionality
 */
UCLASS()
class ACTIONRPG_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPauseMenuWidget(const FObjectInitializer &ObjectInitializer);

	virtual void NativeConstruct() override;

	// Show pause menu
	void ShowPauseMenu();

	// Hide pause menu
	void HidePauseMenu();

protected:
	// Resume button
	UPROPERTY(meta = (BindWidget))
	class UButton *ResumeButton;

	// Quit button
	UPROPERTY(meta = (BindWidget))
	class UButton *QuitButton;

	// Button click handlers
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnQuitClicked();
};
