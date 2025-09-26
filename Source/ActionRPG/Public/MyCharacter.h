// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "MyBaseCharacter.h"
#include "InputActionValue.h"
#include "MyAbilityTypes.h"
#include "MyAttackAbility.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"
#include "MyCharacter.generated.h"

class AMyEnemy;

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UMyAttributeComponent;

UCLASS()
class ACTIONRPG_API AMyCharacter : public AMyBaseCharacter
{
	GENERATED_BODY()
public:
	AMyCharacter();

	// Get the ability system component from the AttributeComponent
	UAbilitySystemComponent *GetAbilitySystem() const
	{
		return AttributeComponent
				   ? AttributeComponent->GetAbilitySystemComponent()
				   : nullptr;
	}

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(
		class UInputComponent *PlayerInputComponent) override;

	// Public getter for sprint status, used by Animation Blueprint
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsSprinting() const
	{
		return AttributeComponent ? AttributeComponent->IsSprinting() : false;
	}

	// Public getter for dodge status, used by Animation Blueprint
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsDodging() const
	{
		return AttributeComponent ? AttributeComponent->IsDodging() : false;
	}

	// Public getter for attack status, used by Animation Blueprint
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsAttacking() const;

	// Get current combo index, used by Animation Blueprint
	UFUNCTION(BlueprintPure, Category = "Character State")
	int32 GetCurrentComboIndex() const;

	// Targeting functions
	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsEnemyInFocusRange(AMyEnemy *Enemy) const
	{
		return NearbyEnemies.Contains(Enemy);
	}

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsEnemyFocused(AMyEnemy *Enemy) const
	{
		return CurrentTarget == Enemy;
	}

	// Handle death - override from base class
	virtual void HandleDeath() override;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController *NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void Landed(const FHitResult &Hit) override;

	// COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent *SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent *CameraComponent;

	// INPUT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext *DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction *FocusEnemyAction;

	// INPUT HANDLERS
	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);
	void StartSprint();
	void StopSprint();
	void Jump();
	void StopJumping();
	void Dodge();
	void Attack();
	void FocusEnemy();

	// Player UI Class
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMyPlayerUI> PlayerUIClass;

	// Player UI Widget
	UPROPERTY()
	UMyPlayerUI *PlayerUIWidget;

	// Dodge cooldown time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	float DodgeCooldown = DefaultValues::DodgeCooldown;

private:
	// Initialize the attribute component
	void InitializeAttributeComponent();

	// Create and setup the player UI
	void InitializePlayerUI();

	// Setup input mapping context
	void SetupInputMapping();

	// Deferred input setup
	void SetupPlayerInputDeferred();

	// Helper to get the active attack ability
	UMyAttackAbility *GetActiveAttackAbility() const;

	// Enemy targeting
	AMyEnemy *CurrentTarget;
	TArray<AMyEnemy *> NearbyEnemies;
	TArray<AMyEnemy *> PreviousNearbyEnemies;
	bool bCameraLocked;
	FTimerHandle UpdateEnemiesTimerHandle;

	void UpdateNearbyEnemies();
	void CycleTarget();
};
