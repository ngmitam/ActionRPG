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

	// Handle death - override from base class
	virtual void HandleDeath() override;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	float MovementSpeed = FGameConfig::GetDefault().EnemyMovementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	float AttackRange = FGameConfig::GetDefault().EnemyAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	float AttackDamage = FGameConfig::GetDefault().EnemyAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	float AttackCooldown = FGameConfig::GetDefault().EnemyAttackCooldown;

	// Patrol points for AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor *> PatrolPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent *HealthBarWidget;

	// Health bar widget class (set in Blueprint)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

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
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float StunDuration = FGameConfig::GetDefault().StunDuration;

	// Delay before destroying after death animation
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float DeathDelay = FGameConfig::GetDefault().DeathLifeSpan;

	// Montage end callback
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	float LastAttackTime = 0.0f;

	ACharacter *PlayerCharacter = nullptr;
};
