// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "MyAbilityTypes.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"
#include "MyAttributeComponent.generated.h"

class UMyAbilitySystemComponent;
class UMyAttributeSet;
class UGameplayEffect;
class UMyGameplayAbility;

UCLASS(ClassGroup = (AbilitySystem), meta = (BlueprintSpawnableComponent, Blueprintable, DisplayName = "My Attribute Component", Category = "Ability System", IsBlueprintBase = "true", Keywords = "Attribute Ability System GAS", ShowCategories = "Ability System", HideCategories = "Tags,Activation,Rendering,AssetUserData,Collision", ShowInBlueprintContextMenu = "true"))
class ACTIONRPG_API UMyAttributeComponent : public UActorComponent, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    UMyAttributeComponent();

    // Implement IAbilitySystemInterface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    virtual void BeginPlay() override;
    virtual void OnRegister() override;

    // Initialize attributes and abilities
    void InitializeAttributes();
    void GiveDefaultAbilities();
    void InitializeAbilitySystem();
    void DeferredInitialize();

    // Attribute accessors
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetStamina() const;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetBaseDamage() const;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxWalkSpeed() const;

    // Public getter for sprint status
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsSprinting() const { return bIsSprinting; }

    // Public getter for dodge status
    UFUNCTION(BlueprintPure, Category = "Character State")
    bool IsDodging() const { return bIsDodging; }

    // Set sprint status (called by abilities)
    void SetSprinting(bool bSprinting) { bIsSprinting = bSprinting; }

    // Set dodge status (called by abilities)
    void SetDodging(bool bDodging) { bIsDodging = bDodging; }

    // Handle death when health <= 0
    void HandleDeath();

protected:
    // Ability System Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAbilitySystemComponent *AbilitySystemComponent;

    // Attribute Set
    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UMyAttributeSet *AttributeSet;

    // Default attribute effect classes
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayEffect>> DefaultAttributeEffectClasses;

    // Default ability classes
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UMyGameplayAbility>> DefaultAbilityClasses;

    // Default attribute values
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultHealth = 100.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultMaxHealth = 100.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultStamina = 100.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultMaxStamina = 100.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultBaseDamage = 10.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0"))
    float DefaultMaxWalkSpeed = 300.0f;

    // Handle attribute changes
    virtual void OnAttributeChange(const FOnAttributeChangeData &Data);
    void OnStaminaChange(const FOnAttributeChangeData &Data);

public:
    // Delegate for health changes
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
    FOnHealthChanged OnHealthChanged;

    // Public accessor for health changed delegate
    FOnHealthChanged &GetOnHealthChanged() { return OnHealthChanged; }

    // Get the attribute set
    UMyAttributeSet *GetAttributeSet() const { return AttributeSet; }

    // Set default attribute values
    void SetDefaultAttributes(float Health = 100.0f, float MaxHealth = 100.0f, float Stamina = 100.0f, float MaxStamina = 100.0f);

private:
    bool bIsSprinting = false;
    bool bIsDodging = false;
};
