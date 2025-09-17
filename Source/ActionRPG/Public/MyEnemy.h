// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

class UMyAttributeComponent;

UCLASS()
class ACTIONRPG_API AMyEnemy : public ACharacter
{
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
    UMyAttributeComponent *GetAttributeComponent() const { return AttributeComponent; }

    // Simple movement for performance
    void MoveTowardsPlayer(float DeltaTime);

    // Attack player if close
    void AttackPlayer();

    // Handle death
    void HandleDeath();

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

private:
    UMyAttributeComponent *AttributeComponent;

    float LastAttackTime = 0.0f;

    ACharacter *PlayerCharacter;
};
