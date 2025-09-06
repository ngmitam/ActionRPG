// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MyCharacter.generated.h"

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

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    virtual void OnRep_PlayerState() override;

    // --- Gameplay Ability System ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAbilitySystemComponent *AbilitySystemComponent;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAttributeSet *AttributeSet;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UMyGameplayAbility>> DefaultAbilities;

    void InitializeAttributes();
    void GiveDefaultAbilities();
};
