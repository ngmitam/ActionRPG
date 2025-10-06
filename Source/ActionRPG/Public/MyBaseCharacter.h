// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAbilityTypes.h"
#include "MyAttributeComponent.h"
#include "MyBaseCharacter.generated.h"

class UMyAttributeComponent;

UCLASS(Abstract)
class ACTIONRPG_API AMyBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyBaseCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Get the ability system component from the AttributeComponent
	UFUNCTION(BlueprintPure, Category = "Abilities")
	UAbilitySystemComponent *GetAbilitySystem() const
	{
		return AttributeComponent
				   ? AttributeComponent->GetAbilitySystemComponent()
				   : nullptr;
	}

	// Get the AttributeComponent
	UFUNCTION(BlueprintPure, Category = "Attributes")
	UMyAttributeComponent *GetAttributeComponent() const
	{
		return AttributeComponent;
	}

	// Check if the attribute component and ability system are valid
	bool IsAttributeSystemValid() const
	{
		return AttributeComponent && AttributeComponent->IsAbilitySystemValid();
	}

	// Get the ability system component safely
	UAbilitySystemComponent *GetAbilitySystemComponent() const
	{
		return AttributeComponent
				   ? AttributeComponent->GetAbilitySystemComponent()
				   : nullptr;
	}

	// Try to get the ability system component, returns nullptr if not available
	UAbilitySystemComponent *TryGetAbilitySystem() const
	{
		return IsAttributeSystemValid() ? GetAbilitySystemComponent() : nullptr;
	}

	// Check if ability system is ready for use
	bool IsAbilitySystemReady() const { return IsAttributeSystemValid(); }

	// Handle damage taken
	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const &DamageEvent,
		class AController *EventInstigator, AActor *DamageCauser) override;

	// Handle death - to be implemented by subclasses
	virtual void HandleDeath();

	// Called when health changes
	void OnHealthChanged(float NewHealth);

	// Initialize default attributes
	virtual void InitializeDefaultAttributes();

	// Death animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *DeathMontage;

	// Attribute component for GAS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UMyAttributeComponent *AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	TSubclassOf<UMyAttributeComponent> AttributeComponentClass =
		UMyAttributeComponent::StaticClass();

	// Default attribute values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultHealth = DefaultValues::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultMaxHealth = DefaultValues::DefaultMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultStamina = DefaultValues::DefaultStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultMaxStamina = DefaultValues::DefaultMaxStamina;

	// Death status
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsDead = false;

protected:
	// Initialize the attribute component
	void InitializeAttributeComponent();

	// Flag to track if default attributes have been initialized
	bool bAttributesInitialized = false;
};
