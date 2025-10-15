// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.generated.h"

class AMyCharacter;

/**
 * Custom Player Controller for ActionRPG with centralized input handling
 */
UCLASS()
class ACTIONRPG_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	virtual void BeginPlay() override;

protected:
	// Input Mapping Context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext *DefaultMappingContext;

	// Character Input Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *DodgeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *FocusEnemyAction;

	// Pause Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction *PauseAction;

	// Pause input handler
	void OnPausePressed();

	// Character input handlers
	void OnMove(const FInputActionValue &Value);
	void OnLook(const FInputActionValue &Value);
	void OnSprintStarted();
	void OnSprintCompleted();
	void OnJumpStarted();
	void OnJumpCompleted();
	void OnDodgeStarted();
	void OnAttackStarted();
	void OnFocusEnemyStarted();

	virtual void SetupInputComponent() override;

private:
	/**
	 * @brief Get the controlled character, cast to AMyCharacter
	 * @return Pointer to the controlled AMyCharacter, or nullptr if not
	 * available
	 */
	AMyCharacter *GetControlledCharacter() const
	{
		return Cast<AMyCharacter>(GetPawn());
	}
};
