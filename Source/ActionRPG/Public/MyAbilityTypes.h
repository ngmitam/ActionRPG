// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EMyAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Sprint UMETA(DisplayName = "Sprint"),
	Jump UMETA(DisplayName = "Jump"),
	Dodge UMETA(DisplayName = "Dodge"),
	Attack UMETA(DisplayName = "Attack")
};

// Gameplay Tag Constants
namespace AbilityTags
{
const FName Sprint = FName("Ability.Sprint");
const FName Jump = FName("Ability.Jump");
const FName Dodge = FName("Ability.Dodge");
const FName Attack = FName("Ability.Attack");
} // namespace AbilityTags

namespace StateTags
{
const FName Sprinting = FName("State.Sprinting");
}

// Component Name Constants
namespace ComponentNames
{
const FName SpringArm = FName("SpringArm");
const FName Camera = FName("Camera");
const FName AttributeComponent = FName("AttributeComponent");
const FName AbilitySystemComponent = FName("AbilitySystemComponent");
const FName HealthBarWidget = FName("HealthBarWidget");
} // namespace ComponentNames

// Default Values
namespace DefaultValues
{
constexpr float SpringArmLength = 350.0f;
constexpr float CameraSocketOffsetX = 0.0f;
constexpr float CameraSocketOffsetY = 60.0f;
constexpr float CameraSocketOffsetZ = 70.0f;
constexpr float CameraLagSpeed = 10.0f;
constexpr float CharacterRotationRate = 500.0f;
constexpr float EnemyRotationRate = 540.0f;
constexpr float DodgeCooldown = 1.0f;
constexpr float DeathLifeSpan = 2.0f;
constexpr int32 MaxInitializationRetries = 10;
constexpr float EnemyMovementSpeed = 100.0f;
constexpr float EnemyAttackRange = 150.0f;
constexpr float EnemyAttackDamage = 5.0f;
constexpr float EnemyAttackCooldown = 2.0f;
constexpr float AttackRange = 200.0f;
constexpr int32 MaxComboCount = 3;
constexpr float DefaultHealth = 100.0f;
constexpr float DefaultMaxHealth = 100.0f;
constexpr float DefaultStamina = 100.0f;
constexpr float DefaultMaxStamina = 100.0f;
constexpr float DefaultBaseDamage = 10.0f;
constexpr float DefaultMaxWalkSpeed = 300.0f;
constexpr float EnemyDetectionRange = 5000.0f;
} // namespace DefaultValues

// Default attribute configuration struct
USTRUCT(BlueprintType)
struct FDefaultAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health = DefaultValues::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHealth = DefaultValues::DefaultMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Stamina = DefaultValues::DefaultStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxStamina = DefaultValues::DefaultMaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float BaseDamage = DefaultValues::DefaultBaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxWalkSpeed = DefaultValues::DefaultMaxWalkSpeed;

	FDefaultAttributes() = default;

	FDefaultAttributes(
		float InHealth, float InMaxHealth, float InStamina, float InMaxStamina)
		: Health(InHealth), MaxHealth(InMaxHealth), Stamina(InStamina),
		  MaxStamina(InMaxStamina)
	{
	}
};