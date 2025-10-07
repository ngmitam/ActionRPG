// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyBoss.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMyBoss::AMyBoss()
{
	// Create spear mesh component
	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpearMesh"));
	SpearMesh->SetupAttachment(
		GetMesh(), TEXT("hand_r")); // Attach to right hand socket

	// Boss is 1.3 times larger
	GetMesh()->SetRelativeScale3D(FVector(1.3f, 1.3f, 1.3f));

	// Boss attacks from farther distance due to spear
	AttackRange = 200.0f; // Increased from Enemy's 150.0f
}

void AMyBoss::BeginPlay()
{
	Super::BeginPlay();
}

void AMyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyBoss::AttackPlayer(ACharacter *Player)
{
	if(!Player)
		return;

	// Boss uses random attack selection
	PerformBossAttack(Player);
}

void AMyBoss::PerformBossAttack(ACharacter *Player)
{
	if(!Player || bIsAttacking)
		return;

	TargetPlayer = Player;

	// Select random attack type
	EBossAttackType AttackType = SelectRandomAttack();

	// Play the attack
	if(PlayAttack(AttackType))
	{
		bIsAttacking = true;

		// Fallback timer to reset attack state if montage doesn't end
		GetWorld()->GetTimerManager().SetTimer(AttackResetTimerHandle, this,
			&AMyBoss::ResetAttackState, 5.0f, false);
	}
}

void AMyBoss::ResetAttackState()
{
	bIsAttacking = false;
	bIsInCombo = false;

	// Clear the fallback timer
	GetWorld()->GetTimerManager().ClearTimer(AttackResetTimerHandle);
}

void AMyBoss::OnComboInterrupted()
{
	if(bIsInCombo)
	{
		// Interrupt combo and stun
		bIsInCombo = false;

		// Stop the current montage
		USkeletalMeshComponent *SkeletalMesh = GetMesh();
		if(SkeletalMesh)
		{
			UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Stop(0.2f); // Blend out over 0.2 seconds
			}
		}

		// Reset attack state since combo is interrupted
		ResetAttackState();

		SetStunned(true);

		// Reset after stun duration
		FTimerHandle StunTimer;
		GetWorld()->GetTimerManager().SetTimer(
			StunTimer, [this]() { SetStunned(false); }, StunDuration, false);
	}
}

float AMyBoss::TakeDamage(float DamageAmount,
	struct FDamageEvent const &DamageEvent, class AController *EventInstigator,
	AActor *DamageCauser)
{
	float ActualDamage = Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Update health bar after damage is applied through GAS
	UpdateHealthBar();

	// Check if dead using GAS health
	if(AttributeComponent && AttributeComponent->GetHealth() <= 0.0f)
	{
		// Death clears stun
		SetStunned(false);
		HandleDeath();
	}
	else
	{
		// Stun only if in combo
		if(bIsInCombo)
		{
			SetStunned(true);
			// Interrupt combo
			OnComboInterrupted();
			// reset bIsInCombo to false
			bIsInCombo = false;
		}
		else
		{
			SetStunned(false);
		}
	}

	return ActualDamage;
}

void AMyBoss::OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
	if(bInterrupted)
	{
		OnComboInterrupted();
	}
	else
	{
		ResetAttackState();
	}
}

EBossAttackType AMyBoss::SelectRandomAttack()
{
	// Random selection: 40% Attack1, 30% Attack2, 30% Combo
	float Rand = FMath::FRand();
	if(Rand < 0.4f)
	{
		return EBossAttackType::Attack1;
	}
	else if(Rand < 0.7f)
	{
		return EBossAttackType::Attack2;
	}
	else
	{
		return EBossAttackType::Combo;
	}
}

bool AMyBoss::PlayAttack(EBossAttackType AttackType)
{
	UAnimMontage *MontageToPlay = nullptr;

	switch(AttackType)
	{
	case EBossAttackType::Attack1:
		MontageToPlay = Attack1Montage;
		bIsInCombo = false;
		break;
	case EBossAttackType::Attack2:
		MontageToPlay = Attack2Montage;
		bIsInCombo = false;
		break;
	case EBossAttackType::Combo:
		MontageToPlay = ComboMontage;
		bIsInCombo = true;
		break;
	}

	if(MontageToPlay)
	{
		USkeletalMeshComponent *SkeletalMesh = GetMesh();
		if(SkeletalMesh)
		{
			UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Stop(0.0f);
				AnimInstance->OnMontageEnded.RemoveDynamic(
					this, &AMyBoss::OnAttackMontageEnded);
				AnimInstance->Montage_Play(MontageToPlay);
				AnimInstance->OnMontageEnded.AddDynamic(
					this, &AMyBoss::OnAttackMontageEnded);
				return true;
			}
		}
	}
	return false;
}

bool AMyBoss::CanBeStunned() const
{
	// Boss can be stunned only if not in combo
	return !bIsInCombo;
}
