// Copyright 2025 ngmitam. All Rights Reserved.

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
	 * @brief Get the ability system component from the AttributeComponent
	 * @return Pointer to the ability system component, or nullptr if not
	 * available
	 */
	UAbilitySystemComponent *GetAbilitySystem() const
	{
		return AttributeComponent
				   ? AttributeComponent->GetAbilitySystemComponent()
				   : nullptr;
	}

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

	// Helper to get the active attack ability
	UMyAttackAbility *GetActiveAttackAbility() const;

	// Enemy targeting
	AMyEnemy *CurrentTarget;
	TArray<AMyEnemy *> NearbyEnemies;
	TArray<AMyEnemy *> PreviousNearbyEnemies;
	bool bCameraLocked;
	FTimerHandle UpdateEnemiesTimerHandle;

	void UpdateNearbyEnemies();
	void FindNearbyEnemies();
	void SortEnemiesByDistance();
	void UpdateHealthBarVisibility();
	void ValidateCurrentTarget();
	void CycleTarget();

	// Helper methods for target cycling
	void HandleNoNearbyEnemies(AMyEnemy *OldTarget);
	TArray<AMyEnemy *> CreateTargetList() const;
	int32 FindCurrentTargetIndex(const TArray<AMyEnemy *> &AllTargets) const;
	AMyEnemy *GetNextTarget(
		const TArray<AMyEnemy *> &AllTargets, int32 CurrentIndex) const;
	void UpdateCameraLock(AMyEnemy *NewTarget);
	void UpdateTargetVisibilityAndFocus(
		AMyEnemy *OldTarget, AMyEnemy *NewTarget);
};
