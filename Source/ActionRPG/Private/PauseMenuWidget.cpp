// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UPauseMenuWidget::UPauseMenuWidget(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if(ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(
			this, &UPauseMenuWidget::OnResumeClicked);
	}

	if(QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(
			this, &UPauseMenuWidget::OnQuitClicked);
	}

	// Initially hidden
	SetVisibility(ESlateVisibility::Hidden);
}

void UPauseMenuWidget::ShowPauseMenu()
{
	SetVisibility(ESlateVisibility::Visible);

	// Pause the game
	if(UWorld *World = GetWorld())
	{
		if(APlayerController *PC = World->GetFirstPlayerController())
		{
			PC->SetPause(true);
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}

void UPauseMenuWidget::HidePauseMenu()
{
	SetVisibility(ESlateVisibility::Hidden);

	// Unpause the game
	if(UWorld *World = GetWorld())
	{
		if(APlayerController *PC = World->GetFirstPlayerController())
		{
			PC->SetPause(false);
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	HidePauseMenu();
}

void UPauseMenuWidget::OnQuitClicked()
{
	// Quit the game
	if(UWorld *World = GetWorld())
	{
		if(APlayerController *PC = World->GetFirstPlayerController())
		{
			UKismetSystemLibrary::QuitGame(
				World, PC, EQuitPreference::Quit, false);
		}
	}
}
