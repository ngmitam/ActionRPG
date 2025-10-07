// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyEnemy.h"
#include "MyBoss.generated.h"

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

	// Override attack to use boss-specific attacks
	virtual void AttackPlayer(ACharacter *Player) override;

	// Perform a boss attack
	void PerformBossAttack(ACharacter *Player);

	// Reset attack state
	void ResetAttackState();

	// Handle combo interruption
	void OnComboInterrupted();

	// Override TakeDamage for boss-specific behavior
	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const &DamageEvent,
		class AController *EventInstigator, AActor *DamageCauser) override;

	// Montage end callback
	void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

	// Select random attack type
	EBossAttackType SelectRandomAttack();

	// Play the selected attack
	bool PlayAttack(EBossAttackType AttackType);

	// Check if boss can be stunned
	bool CanBeStunned() const;

	// Spear mesh for boss
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent *SpearMesh;

	// Attack montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *Attack1Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *Attack2Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage *ComboMontage;

	// Animation state
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsInCombo = false;

	// Target player for attacks
	ACharacter *TargetPlayer = nullptr;

	// Timer handle for attack reset
	FTimerHandle AttackResetTimerHandle;
};
