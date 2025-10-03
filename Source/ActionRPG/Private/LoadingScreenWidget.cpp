// Copyright 2025 ngmitam. All Rights Reserved.

#include "LoadingScreenWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

ULoadingScreenWidget::ULoadingScreenWidget(
	const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize with default text
	if(LoadingTextBlock)
	{
		LoadingTextBlock->SetText(FText::FromString(TEXT("Loading...")));
	}

	if(LoadingProgressBar)
	{
		LoadingProgressBar->SetPercent(0.0f);
	}
}

void ULoadingScreenWidget::ShowLoadingScreen(const FText &LoadingText)
{
	CurrentLoadingText = LoadingText;

	if(LoadingTextBlock)
	{
		LoadingTextBlock->SetText(LoadingText);
	}

	SetVisibility(ESlateVisibility::Visible);
}

void ULoadingScreenWidget::HideLoadingScreen()
{
	SetVisibility(ESlateVisibility::Hidden);
}
