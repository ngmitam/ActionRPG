// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "MyAbilitySystemComponent.h"
#include "MyAbilityTypes.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"
#include "MyAttributeComponent.generated.h"

class UMyAbilitySystemComponent;
class UMyAttributeSet;
class UGameplayEffect;
class UMyGameplayAbility;

UCLASS(ClassGroup = (AbilitySystem),
	meta = (BlueprintSpawnableComponent, Blueprintable,
		DisplayName = "My Attribute Component", Category = "Ability System",
		IsBlueprintBase = "true", Keywords = "Attribute Ability System GAS",
		ShowCategories = "Ability System",
		HideCategories = "Tags,Activation,Rendering,AssetUserData,Collision",
		ShowInBlueprintContextMenu = "true"))
class ACTIONRPG_API UMyAttributeComponent : public UActorComponent,
											public IAbilitySystemInterface
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

	// Helper methods for initialization
	bool IsReadyForInitialization() const;
	void ScheduleRetryInitialization();
	bool CanInitializeAbilitySystem() const;
	void SetupAttributeChangeDelegates();
	void SetInitialCharacterMovement();

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
	float GetMaxWalkSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetStunDuration() const;

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

	// Delegate for health changes
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
	FOnHealthChanged OnHealthChanged;

	// Public accessor for health changed delegate
	FOnHealthChanged &GetOnHealthChanged() { return OnHealthChanged; }

	// Get the attribute set
	UMyAttributeSet *GetAttributeSet() const { return AttributeSet; }

	// Check if the ability system is fully initialized and valid
	bool IsAbilitySystemValid() const
	{
		return AbilitySystemComponent
			   && AbilitySystemComponent->AbilityActorInfo.IsValid();
	}

	// Helper validation methods
	bool HasValidAttributeSet() const { return AttributeSet != nullptr; }
	bool HasValidOwner() const { return GetOwner() != nullptr; }
	bool IsAttributeInitialized(const FGameplayAttribute &Attribute) const;

	// Set default attribute values
	void SetDefaultAttributes(float Health = 100.0f, float MaxHealth = 100.0f,
		float Stamina = 100.0f, float MaxStamina = 100.0f);

	// Set default attribute values using struct
	void SetDefaultAttributes(const FDefaultAttributes &Attributes);

	float GetDefaultMaxWalkSpeed() const { return DefaultMaxWalkSpeed; }

protected:
	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities",
		meta = (AllowPrivateAccess = "true"))
	UMyAbilitySystemComponent *AbilitySystemComponent;

	// Attribute Set
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly,
		Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UMyAttributeSet *AttributeSet;

	// Default attribute effect classes
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> DefaultAttributeEffectClasses;

	// Default ability classes
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UMyGameplayAbility>> DefaultAbilityClasses;

	// Default attribute values
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes",
		meta = (ClampMin = "0.0"))
	float DefaultHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes",
		meta = (ClampMin = "0.0"))
	float DefaultMaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes",
		meta = (ClampMin = "0.0"))
	float DefaultStamina = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes",
		meta = (ClampMin = "0.0"))
	float DefaultMaxStamina = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attributes",
		meta = (ClampMin = "0.0"))
	float DefaultMaxWalkSpeed = 300.0f;

	// Handle attribute changes
	virtual void OnAttributeChange(const FOnAttributeChangeData &Data);
	void OnStaminaChange(const FOnAttributeChangeData &Data);
	// Validate that the owner is properly initialized
	bool ValidateOwner() const;

	// Initialize GAS components (AttributeSet, replication)
	void InitializeGASComponents();

	// Check if ability system component is registered
	bool IsAbilitySystemComponentRegistered() const;

	// Check if ability actor info is valid
	bool IsAbilityActorInfoValid() const;

	// Try to initialize ability actor info
	bool TryInitializeAbilityActorInfo();

	// Handle initialization retry logic
	void HandleInitializationRetry();

	bool bIsSprinting = false;
	bool bIsDodging = false;
};
