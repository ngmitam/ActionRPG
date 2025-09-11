// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
	/** Getter for attribute on the base class */              \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	/** Returns current value of attribute */                  \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
	/** Sets current value of attribute */                     \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
	/** Initializes current value of attribute */              \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ACTIONRPG_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UMyAttributeSet();

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	// Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health);

	// Max Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth);

	// Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Stamina);

	// Max Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Stamina", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxStamina);

	// Damage
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Damage", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, BaseDamage);

	// Current Walk Speed
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Movement", ReplicatedUsing = OnRep_MaxWalkSpeed)
	FGameplayAttributeData MaxWalkSpeed;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxWalkSpeed);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData &OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData &OldMaxHealth);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData &OldStamina);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData &OldMaxStamina);

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData &OldBaseDamage);

	UFUNCTION()
	void OnRep_MaxWalkSpeed(const FGameplayAttributeData &OldMaxWalkSpeed);
};
