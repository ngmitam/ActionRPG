// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "MyBaseCharacter.h"
#include "InputActionValue.h"
#include "MyAbilityTypes.h"
#include "MyGameConfig.h"
#include "MyAttackAbility.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"
#include "MyCharacter.generated.h"

class AMyEnemy;

class USpringArmComponent;
class UCameraComponent;
class UMyAttributeComponent;

UCLASS()
/**
 * @class AMyCharacter
 * @brief Player character class with GAS integration, combo attacks, and enemy
 * targeting
 *
 * This class extends AMyBaseCharacter to provide player-specific functionality
 * including:
 * - Input handling for movement, combat, and abilities
 * - Enemy targeting and camera locking system
 * - Player UI management
 * - Sprint, jump, dodge, and attack abilities
 */
class ACTIONRPG_API AMyCharacter : public AMyBaseCharacter
{
	GENERATED_BODY()
public:
	AMyCharacter();

	/**
	 * @brief Called every frame
	 * @param DeltaTime Time elapsed since last frame
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Get sprint status for Animation Blueprint
	 * @return True if character is sprinting
	 */
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsSprinting() const
	{
		return AttributeComponent ? AttributeComponent->IsSprinting() : false;
	}

	/**
	 * @brief Get dodge status for Animation Blueprint
	 * @return True if character is dodging
	 */
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsDodging() const
	{
		return AttributeComponent ? AttributeComponent->IsDodging() : false;
	}

	/**
	 * @brief Get attack status for Animation Blueprint
	 * @return True if character is currently attacking
	 */
	UFUNCTION(BlueprintPure, Category = "Character State")
	bool IsAttacking() const;

	/**
	 * @brief Get current combo index for Animation Blueprint
	 * @return Current combo index in attack sequence, 0 if not attacking
	 */
	UFUNCTION(BlueprintPure, Category = "Character State")
	int32 GetCurrentComboIndex() const;

	/**
	 * @brief Check if enemy is in focus range
	 * @param Enemy The enemy to check
	 * @return True if enemy is in nearby enemies list
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsEnemyInFocusRange(AMyEnemy *Enemy) const
	{
		return NearbyEnemies.Contains(Enemy);
	}

	/**
	 * @brief Check if enemy is currently focused/targeted
	 * @param Enemy The enemy to check
	 * @return True if enemy is the current target
	 */
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

	// INPUT HANDLERS - Now called by PlayerController
public:
	/**
	 * @brief Handle movement input
	 * @param Value Input action value containing movement vector
	 */
	void Move(const FInputActionValue &Value);

	/**
	 * @brief Handle look/camera input
	 * @param Value Input action value containing look axis
	 */
	void Look(const FInputActionValue &Value);

	/**
	 * @brief Start sprinting ability
	 */
	void StartSprint();

	/**
	 * @brief Stop sprinting ability
	 */
	void StopSprint();

	/**
	 * @brief Perform jump action
	 */
	void Jump();

	/**
	 * @brief Stop jumping ability
	 */
	void StopJumping();

	/**
	 * @brief Perform dodge ability
	 */
	void Dodge();

	/**
	 * @brief Perform attack action
	 */
	void Attack();

	/**
	 * @brief Cycle focus to next enemy target
	 */
	void FocusEnemy();

	// Player UI Class
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMyPlayerUI> PlayerUIClass;

	// Player UI Widget
	UPROPERTY()
	UMyPlayerUI *PlayerUIWidget;

	// Dodge cooldown time
	UPROPERTY(BlueprintReadOnly, Category = "Dodge")
	float DodgeCooldown = FGameConfig::GetDefault().DodgeCooldown;

private:
	/**
	 * @brief Initialize the attribute component
	 */
	void InitializeAttributeComponent();

	/**
	 * @brief Create and setup the player UI
	 */
	void InitializePlayerUI();

	/**
	 * @brief Helper to get the active attack ability
	 * @return Pointer to active attack ability, nullptr if none
	 */
	UMyAttackAbility *GetActiveAttackAbility() const;

	/**
	 * @brief Check if character can perform abilities (not attacking and
	 * ability system ready)
	 * @return True if character can perform abilities
	 */
	bool CanPerformAbility() const
	{
		return !IsAttacking() && IsAbilitySystemReady();
	}

	/**
	 * @brief Dodge reset helper - called by timer
	 */
	void ResetDodgeStatus();

	/**
	 * @brief Level reset helper - called by timer after death
	 */
	void ResetLevel();

	/**
	 * @brief Get the player controller, cast to APlayerController
	 * @return Pointer to the player controller, or nullptr if not available
	 */
	APlayerController *GetPlayerController() const
	{
		return Cast<APlayerController>(Controller);
	}

	// Enemy targeting
	AMyEnemy *CurrentTarget;
	TArray<AMyEnemy *> NearbyEnemies;
	TArray<AMyEnemy *> PreviousNearbyEnemies;
	bool bCameraLocked;
	FTimerHandle UpdateEnemiesTimerHandle;

	/**
	 * @brief Update the list of nearby enemies and their UI state
	 */
	void UpdateNearbyEnemies();

	/**
	 * @brief Find all enemies within detection range
	 */
	void FindNearbyEnemies();

	/**
	 * @brief Sort nearby enemies by distance from character
	 */
	void SortEnemiesByDistance();

	/**
	 * @brief Update health bar visibility for nearby enemies
	 */
	void UpdateHealthBarVisibility();

	/**
	 * @brief Validate current target and cycle if invalid
	 */
	void ValidateCurrentTarget();

	/**
	 * @brief Set the current target enemy with proper state management
	 * @param NewTarget The enemy to target, nullptr to clear target
	 */
	void SetTarget(AMyEnemy *NewTarget);

	/**
	 * @brief Clear the current target enemy
	 */
	void ClearTarget();

	/**
	 * @brief Cycle to the next enemy target
	 */
	void CycleTarget();
};
