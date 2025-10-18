// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "MyGameConfig.generated.h"

// Centralized game configuration struct
USTRUCT(BlueprintType)
struct FGameConfig
{
	GENERATED_BODY()

	// Character Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultMaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultMaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float DefaultMaxWalkSpeed = 300.0f;

	// Enemy Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyMovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyAttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyAttackDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyAttackCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyDetectionRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float EnemyUpdateInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float PatrolPointTolerance = 100.0f;

	// Gameplay Constants
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float GroundZCoordinate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DefaultMovementInput = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DefaultAbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float StunDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DeathLifeSpan = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DeathResetDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float LoadingScreenDelay = 2.0f;

	// Character Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CharacterRotationRate = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float EnemyRotationRate = 540.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DodgeCooldown = 1.0f;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRangeTolerance = 50.0f;

	// Attack Trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTraceLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTraceOffsetX = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTraceOffsetY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTraceOffsetZ = 0.0f;

	// Knockback
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float KnockbackForce = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxComboCount = 3;

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SpringArmLength = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraSocketOffsetX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraSocketOffsetY = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraSocketOffsetZ = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLagSpeed = 10.0f;

	// Initialization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	int32 MaxInitializationRetries = 10;

	// UI Constants
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float HealthHighThreshold = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float HealthMediumThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float StaminaHighThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float StaminaMediumThreshold = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float HealthBarWidth = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float HealthBarHeight = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float HealthBarZOffset = 100.0f;

	// UI Z-Orders
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 LoadingScreenZOrder = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 PauseMenuZOrder = 50;

	// UI Colors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor HealthColorHigh = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor HealthColorMedium =
		FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor HealthColorLow = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor StaminaColorHigh = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor StaminaColorMedium =
		FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // Orange

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor StaminaColorLow = FLinearColor::Red;

	FGameConfig() = default;

	// Static default config instance
	static const FGameConfig &GetDefault()
	{
		static FGameConfig DefaultConfig;
		return DefaultConfig;
	}
};

// Default attribute configuration struct
USTRUCT(BlueprintType)
struct FDefaultAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxWalkSpeed = 300.0f;

	FDefaultAttributes() = default;

	FDefaultAttributes(
		float InHealth, float InMaxHealth, float InStamina, float InMaxStamina)
		: Health(InHealth), MaxHealth(InMaxHealth), Stamina(InStamina),
		  MaxStamina(InMaxStamina)
	{
	}

	// Initialize from game config
	void InitializeFromConfig(const FGameConfig &Config)
	{
		Health = Config.DefaultHealth;
		MaxHealth = Config.DefaultMaxHealth;
		Stamina = Config.DefaultStamina;
		MaxStamina = Config.DefaultMaxStamina;
		MaxWalkSpeed = Config.DefaultMaxWalkSpeed;
	}
};
