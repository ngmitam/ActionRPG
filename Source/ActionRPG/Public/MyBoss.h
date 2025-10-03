// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyEnemy.h"
#include "MyBoss.generated.h"

class UMyAttributeComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	Attack1 UMETA(DisplayName = "Attack1"),
	Attack2 UMETA(DisplayName = "Attack2"),
	Combo UMETA(DisplayName = "Combo")
};

UCLASS()
class ACTIONRPG_API AMyBoss : public AMyEnemy
{
	GENERATED_BODY()

public:
	AMyBoss();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Override attack to implement random attacks
	virtual void AttackPlayer(ACharacter *Player) override;

	// Boss attack with random selection
	void PerformBossAttack(ACharacter *Player);

	// Reset attack state
	void ResetAttackState();

	// Handle combo interruption for stun
	void OnComboInterrupted();

	// Initialize default attributes with higher health
	virtual void InitializeDefaultAttributes() override;

	// Spear component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent *SpearMesh;

	// Boss specific properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float BossMaxHealth = 300.0f; // Higher health

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float BossAttackDamage = 15.0f;

	// Attack montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *Attack1Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *Attack2Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *ComboMontage;

	// Target player for attack
	UPROPERTY()
	ACharacter *TargetPlayer;

	// Attack notify
	UFUNCTION(BlueprintCallable)
	void OnAttackNotify();

	// Animation state
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	EBossAttackType CurrentAttackType = EBossAttackType::Attack1;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsInCombo = false;

private:
	// Montage end callback
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	// Select random attack type
	EBossAttackType SelectRandomAttack();

	// Play selected attack
	void PlayAttack(EBossAttackType AttackType);

	// Apply damage to player
	void ApplyDamageToPlayer(ACharacter *Player);

	// Check if can be stunned
	bool CanBeStunned() const;

private:
	// Timer handle for attack reset
	FTimerHandle AttackResetTimerHandle;
};
