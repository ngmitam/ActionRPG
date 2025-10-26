// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameMode.h"
#include "MyCharacter.h"

AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if(UEnhancedInputLocalPlayerSubsystem *Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				GetLocalPlayer()))
	{
		if(DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyPlayerController::OnPausePressed()
{
	if(AMyGameMode *GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->TogglePauseMenu();
	}
}

void AMyPlayerController::OnMove(const FInputActionValue &Value)
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->Move(Value);
	}
}

void AMyPlayerController::OnLook(const FInputActionValue &Value)
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->Look(Value);
	}
}

void AMyPlayerController::OnSprintStarted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->StartSprint();
	}
}

void AMyPlayerController::OnSprintCompleted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->StopSprint();
	}
}

void AMyPlayerController::OnJumpStarted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->Jump();
	}
}

void AMyPlayerController::OnJumpCompleted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->StopJumping();
	}
}

void AMyPlayerController::OnDodgeStarted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->Dodge();
	}
}

void AMyPlayerController::OnAttackStarted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->Attack();
	}
}

void AMyPlayerController::OnFocusEnemyStarted()
{
	if(AMyCharacter *MyCharacter = GetControlledCharacter())
	{
		MyCharacter->FocusEnemy();
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent *EnhancedInputComponent =
			Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Character input actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &AMyPlayerController::OnMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &AMyPlayerController::OnLook);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
			this, &AMyPlayerController::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction,
			ETriggerEvent::Completed, this,
			&AMyPlayerController::OnSprintCompleted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &AMyPlayerController::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &AMyPlayerController::OnJumpCompleted);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started,
			this, &AMyPlayerController::OnDodgeStarted);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
			this, &AMyPlayerController::OnAttackStarted);
		EnhancedInputComponent->BindAction(FocusEnemyAction,
			ETriggerEvent::Started, this,
			&AMyPlayerController::OnFocusEnemyStarted);

		// Pause action
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started,
			this, &AMyPlayerController::OnPausePressed);
	}
}
