// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "MyAbilityTypes.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"
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

    // INPUT HANDLERS
    void Move(const FInputActionValue &Value);
    void Look(const FInputActionValue &Value);
    void StartSprint();
    void StopSprint();
    void Jump();
    void StopJumping();
    void Dodge();

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
};
