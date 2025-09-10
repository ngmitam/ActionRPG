// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "MyAbilityTypes.h"
#include "MyAttributeComponent.h"
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

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

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

    // INPUT HANDLERS
    void Move(const FInputActionValue &Value);
    void Look(const FInputActionValue &Value);
    void StartSprint();
    void StopSprint();
    void Jump();
    void StopJumping();

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
    TSubclassOf<UMyAttributeComponent> AttributeComponentClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    UMyAttributeComponent *AttributeComponent;

    // Public getter for sprint status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsSprinting() const { return AttributeComponent ? AttributeComponent->IsSprinting() : false; }
};
