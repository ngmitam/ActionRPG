// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayAbility.h"
#include "MyAttackAbility.generated.h"

class UAnimMontage;

/**
 * Attack Ability with combo system
 */
UCLASS()
class ACTIONRPG_API UMyAttackAbility : public UMyGameplayAbility
{
	GENERATED_BODY()

public:
	UMyAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData *TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	// Called when attack input is pressed
	void OnAttackInputPressed();

	// Check if we can combo to next attack
	bool CanCombo() const;

	// Advance to next combo attack
	void AdvanceCombo();

	// Reset combo state
	void ResetCombo();

	// Start combo window
	void StartComboWindow();

	// End combo window
	void EndComboWindow();

	// Get current combo index
	int32 GetCurrentComboIndex() const { return CurrentComboIndex; }

	// Get max combo index (MaxComboCount - 1)
	int32 GetMaxComboIndex() const { return MaxComboCount - 1; }

protected:
	// Combo system properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 MaxComboCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<UAnimMontage *> AttackMontages; // Montages for each combo level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<TSubclassOf<UGameplayEffect>>
		DamageEffects; // Damage effects for each combo level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange = 200.0f;

	// Current combo state
	int32 CurrentComboIndex = 0;

	// Whether we're in combo window
	bool bInComboWindow = false;

	// Whether next combo is queued
	bool bComboQueued = false;

	// Play attack montage
	void PlayAttackMontage();

	// Apply damage effect
	void ApplyDamageEffect();

	// Find target in attack range
	AActor *FindTargetInRange();

	// Handle montage ended
	void OnMontageEnded(UAnimMontage *Montage, bool bInterrupted);
};
