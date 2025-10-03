// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "MyBaseCharacter.h"
#include "MyEnemy.generated.h"

class UMyAttributeComponent;
class AAIController;

UCLASS()
class ACTIONRPG_API AMyEnemy : public AMyBaseCharacter
{
	GENERATED_BODY()

public:
	AMyEnemy();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Get the ability system component from the AttributeComponent
	UAbilitySystemComponent *GetAbilitySystem() const;

	// Simple movement for performance
	void MoveTowardsPlayer(float DeltaTime);

	// Attack player if close
	virtual void AttackPlayer(ACharacter *Player);

	// Reset attack state (called from animation notify)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void ResetAttackState();

	// Initialize health bar widget
	void InitializeHealthBar();

	// Update health bar widget
	void UpdateHealthBar();

	// Handle enemy-specific health changes
	void OnEnemyHealthChanged(float NewHealth);

	// Simple health accessors (not using GAS)
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxHealth() const { return MaxHealth; }

	// Apply damage
	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const &DamageEvent,
		class AController *EventInstigator, AActor *DamageCauser) override;

	// Set stun state (for animation)
	void SetStunned(bool bStunned);

	// Set health bar visibility
	void SetHealthBarVisible(bool bVisible);

	// Set focused state
	void SetFocused(bool bFocused);

	// Initialize default attributes
	void InitializeDefaultAttributes() override;

	// Handle death - override from base class
	virtual void HandleDeath() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MovementSpeed = DefaultValues::EnemyMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = DefaultValues::EnemyAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackDamage = DefaultValues::EnemyAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackCooldown = DefaultValues::EnemyAttackCooldown;

	// Patrol points for AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor *> PatrolPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent *HealthBarWidget;

	// Health bar widget class (set in Blueprint)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *AttackMontage;

	// Animation state variables for ABP
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsAttacking = false;

	// Stun state for animation blueprint
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsStunned = false;

	// Focused state for health bar color
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bIsFocused = false;

	// Stun duration when taking damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float StunDuration = 2.0f;

	// Delay before destroying after death animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float DeathDelay = 5.0f;

	// Montage end callback
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	float LastAttackTime = 0.0f;

	ACharacter *PlayerCharacter = nullptr;
};
