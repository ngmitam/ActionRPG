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

	// Boss has higher health
	MaxHealth = BossMaxHealth;
	Health = BossMaxHealth;
	AttackDamage = BossAttackDamage;

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
	CurrentAttackType = AttackType;

	// Play the attack
	PlayAttack(AttackType);

	bIsAttacking = true;
}

void AMyBoss::ResetAttackState()
{
	bIsAttacking = false;
	bIsInCombo = false;
	CurrentAttackType = EBossAttackType::Attack1;
}

void AMyBoss::OnComboInterrupted()
{
	if(bIsInCombo)
	{
		// Interrupt combo and stun
		bIsInCombo = false;
		SetStunned(true);

		// Reset after stun duration
		FTimerHandle StunTimer;
		GetWorld()->GetTimerManager().SetTimer(
			StunTimer, [this]() { SetStunned(false); }, StunDuration, false);
	}
}

void AMyBoss::InitializeDefaultAttributes()
{
	// Boss has higher health
	MaxHealth = BossMaxHealth;
	Health = BossMaxHealth;
	AttackDamage = BossAttackDamage;
}

void AMyBoss::OnAttackNotify()
{
	// Apply damage to target player
	if(TargetPlayer)
	{
		ApplyDamageToPlayer(TargetPlayer);
	}
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

void AMyBoss::PlayAttack(EBossAttackType AttackType)
{
	UAnimMontage *MontageToPlay = nullptr;

	switch(AttackType)
	{
	case EBossAttackType::Attack1:
		MontageToPlay = Attack1Montage;
		break;
	case EBossAttackType::Attack2:
		MontageToPlay = Attack2Montage;
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
			}
		}
	}
}

void AMyBoss::ApplyDamageToPlayer(ACharacter *Player)
{
	if(!Player)
		return;

	// Apply damage using GameplayStatics, which will call TakeDamage on the
	// player
	UGameplayStatics::ApplyDamage(Player, AttackDamage, nullptr, this, nullptr);
}

bool AMyBoss::CanBeStunned() const
{
	// Boss can be stunned only if not in combo
	return !bIsInCombo;
}
