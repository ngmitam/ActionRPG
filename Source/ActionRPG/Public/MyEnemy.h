// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

class UMyAttributeComponent;

UCLASS()
class ACTIONRPG_API AMyEnemy : public ACharacter {
  GENERATED_BODY()

public:
  AMyEnemy();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  // Get the ability system component from the AttributeComponent
  UFUNCTION(BlueprintPure, Category = "Abilities")
  UAbilitySystemComponent *GetAbilitySystem() const;

  // Get the AttributeComponent
  UFUNCTION(BlueprintPure, Category = "Attributes")
  UMyAttributeComponent *GetAttributeComponent() const {
    return AttributeComponent;
  }

  // Simple movement for performance
  void MoveTowardsPlayer(float DeltaTime);

  // Attack player if close
  void AttackPlayer();

  // Handle death
  void HandleDeath();

  // Update health bar widget
  void UpdateHealthBar();

  // Called when health changes
  void OnHealthChanged(float NewHealth);

  // Initialize default attributes
  void InitializeDefaultAttributes();

  // Initialize health bar widget
  void InitializeHealthBar();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
  float MovementSpeed = 100.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
  float AttackRange = 150.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
  float AttackDamage = 5.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
  float AttackCooldown = 2.0f;

  // Death animation montage
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  UAnimMontage *DeathMontage;

  // Attack animation montage
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  UAnimMontage *AttackMontage;

  // Idle animation montage (optional, for performance)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  UAnimMontage *IdleMontage;

  // Walk animation montage (optional, for performance)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  UAnimMontage *WalkMontage;

  // Health bar widget component
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
  UWidgetComponent *HealthBarWidget;

  // Attribute component for GAS
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
  UMyAttributeComponent *AttributeComponent;

  // Health bar widget class (set in Blueprint)
  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
  TSubclassOf<UUserWidget> HealthBarWidgetClass;

  // Default attribute values (set in Blueprint)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
  float DefaultHealth = 100.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
  float DefaultMaxHealth = 100.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
  float DefaultStamina = 100.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
  float DefaultMaxStamina = 100.0f;

  // Play movement animation based on movement state
  void PlayMovementAnimation(bool bMoving);

private:
  float LastAttackTime = 0.0f;

  ACharacter *PlayerCharacter;

  // Previous health value to detect changes
  float PreviousHealth = -1.0f;

  // Animation state tracking for performance
  bool bIsMoving = false;
  UAnimMontage *CurrentMontage = nullptr;
};
