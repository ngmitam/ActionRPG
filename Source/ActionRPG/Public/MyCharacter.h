// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "MyAbilityTypes.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"
#include "MyAttackAbility.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UMyAttributeComponent;

UCLASS()
class ACTIONRPG_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AMyCharacter();

    // Get the ability system component from the AttributeComponent
    UFUNCTION(BlueprintPure, Category = "Abilities")
    UAbilitySystemComponent *GetAbilitySystem() const { return AttributeComponent ? AttributeComponent->GetAbilitySystemComponent() : nullptr; }

    // Get the AttributeComponent (for UI access)
    UFUNCTION(BlueprintPure, Category = "Attributes")
    UMyAttributeComponent *GetAttributeComponent() const { return AttributeComponent; }

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    // Public getter for sprint status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsSprinting() const { return AttributeComponent ? AttributeComponent->IsSprinting() : false; }

    // Public getter for dodge status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsDodging() const { return AttributeComponent ? AttributeComponent->IsDodging() : false; }

    // Public getter for attack status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsAttacking() const;

    // Get current combo index, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    int32 GetCurrentComboIndex() const;

    // Public getter for death status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsDead() const { return bIsDead; }

    // Handle death
    void HandleDeath();

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

    // INPUT HANDLERS
    void Move(const FInputActionValue &Value);
    void Look(const FInputActionValue &Value);
    void StartSprint();
    void StopSprint();
    void Jump();
    void StopJumping();
    void Dodge();
    void Attack();

    // Deferred input setup
    void SetupPlayerInputDeferred();

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
    TSubclassOf<UMyAttributeComponent> AttributeComponentClass;

    // Attribute Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    UMyAttributeComponent *AttributeComponent;

    // Player UI Class
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMyPlayerUI> PlayerUIClass;

    // Player UI Widget
    UPROPERTY()
    UMyPlayerUI *PlayerUIWidget;

    // Dodge cooldown time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
    float DodgeCooldown = 1.0f;

    // Death animation montage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage *DeathMontage;

private:
    // Death status
    bool bIsDead = false;
};
