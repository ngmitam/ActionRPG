// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * Widget for displaying loading screens
 */
UCLASS()
class ACTIONRPG_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULoadingScreenWidget(const FObjectInitializer &ObjectInitializer);

	virtual void NativeConstruct() override;

	// Show loading screen with optional text
	void ShowLoadingScreen(
		const FText &LoadingText = FText::FromString(TEXT("Loading...")));

	// Hide loading screen
	void HideLoadingScreen();

protected:
	// Text block for loading message
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *LoadingTextBlock;

	// Progress bar for loading progress (optional)
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *LoadingProgressBar;

private:
	// Current loading text
	FText CurrentLoadingText;
};
