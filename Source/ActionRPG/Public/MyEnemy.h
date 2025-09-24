// Fill out your copyright notice in the Description page of Project Settings.

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
	void AttackPlayer(ACharacter *Player);

	// Reset attack state (called from animation notify)
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void ResetAttackState();

	// Initialize health bar widget
	void InitializeHealthBar();

	// Update health bar widget
	void UpdateHealthBar();

	// Handle enemy-specific health changes
	void OnEnemyHealthChanged(float NewHealth);

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

	// Health bar widget component
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

private:
	// Montage end callback
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	float LastAttackTime = 0.0f;

	ACharacter *PlayerCharacter;

	// Previous health value to detect changes
	float PreviousHealth = -1.0f;
};
