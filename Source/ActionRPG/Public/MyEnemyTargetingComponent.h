// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyGameConfig.h"
#include "MyEnemyTargetingComponent.generated.h"

class AMyEnemy;

/**
 * Component responsible for enemy targeting logic
 * Handles finding nearby enemies, sorting by distance, and managing target
 * selection
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UMyEnemyTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyEnemyTargetingComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction *ThisTickFunction) override;

	/**
	 * Get the currently targeted enemy
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	AMyEnemy *GetCurrentTarget() const { return CurrentTarget; }

	/**
	 * Check if enemy is in focus range
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsEnemyInFocusRange(AMyEnemy *Enemy) const
	{
		return NearbyEnemies.Contains(Enemy);
	}

	/**
	 * Check if enemy is currently focused/targeted
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsEnemyFocused(AMyEnemy *Enemy) const
	{
		return CurrentTarget == Enemy;
	}

	/**
	 * Focus the nearest enemy or cycle to next target
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void FocusEnemy();

	/**
	 * Clear the current target
	 */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ClearTarget();

	/**
	 * Get all nearby enemies
	 */
	UFUNCTION(BlueprintPure, Category = "Targeting")
	const TArray<AMyEnemy *> &GetNearbyEnemies() const { return NearbyEnemies; }

private:
	/** Currently targeted enemy */
	UPROPERTY()
	AMyEnemy *CurrentTarget;

	/** List of nearby enemies within detection range */
	UPROPERTY()
	TArray<AMyEnemy *> NearbyEnemies;

	/** Previous frame's nearby enemies for comparison */
	UPROPERTY()
	TArray<AMyEnemy *> PreviousNearbyEnemies;

	/** Timer handle for periodic enemy updates */
	FTimerHandle UpdateEnemiesTimerHandle;

	/** Camera lock state */
	bool bCameraLocked;

	/**
	 * Update the list of nearby enemies and their UI state
	 */
	void UpdateNearbyEnemies();

	/**
	 * Find all enemies within detection range
	 */
	void FindNearbyEnemies();

	/**
	 * Sort nearby enemies by distance from owner
	 */
	void SortEnemiesByDistance();

	/**
	 * Update health bar visibility for nearby enemies
	 */
	void UpdateHealthBarVisibility();

	/**
	 * Validate current target and cycle if invalid
	 */
	void ValidateCurrentTarget();

	/**
	 * Set the current target enemy with proper state management
	 */
	void SetTarget(AMyEnemy *NewTarget);

	/**
	 * Cycle to the next enemy target
	 */
	void CycleTarget();
};
