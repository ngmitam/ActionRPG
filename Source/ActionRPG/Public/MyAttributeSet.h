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
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health);

	// Max Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth);

	// Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Stamina);

	// Max Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxStamina);

	// Damage
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Damage")
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, BaseDamage);

	// Current Walk Speed
	UPROPERTY(BlueprintReadOnly, Category = "Attributes | Movement", ReplicatedUsing = OnRep_MaxWalkSpeed)
	FGameplayAttributeData MaxWalkSpeed;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxWalkSpeed);

	UFUNCTION()
	void OnRep_MaxWalkSpeed(const FGameplayAttributeData &OldMaxWalkSpeed);
};
