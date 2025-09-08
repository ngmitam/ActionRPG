// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "MyAbilityTypes.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UMyAbilitySystemComponent;
class UMyAttributeSet;
class UGameplayEffect;
class UMyGameplayAbility;

UCLASS()
class ACTIONRPG_API AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()
public:
    AMyCharacter();

    // Implement IAbilitySystemInterface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    virtual void OnRep_PlayerState() override;

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

    // --- Gameplay Ability System ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAbilitySystemComponent *AbilitySystemComponent;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAttributeSet *AttributeSet;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayEffect>> DefaultAttributeEffects;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UMyGameplayAbility>> DefaultAbilities;

    void InitializeAttributes();
    void GiveDefaultAbilities();

    // handle callback Attribute changes
    virtual void onAttributeChange(const FOnAttributeChangeData &Data);
    void onStaminaChange(const FOnAttributeChangeData &Data);

    // Public getter for sprint status, used by Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsSprinting() const { return bIsSprinting; }

private:
    bool bIsSprinting = false;
};
