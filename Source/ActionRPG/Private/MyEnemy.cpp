// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyEnemy.h"

#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "MyEnemyAIController.h"

AMyEnemy::AMyEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set AI Controller class
	AIControllerClass = AMyEnemyAIController::StaticClass();

	// Simple movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate =
		FRotator(0.0f, DefaultValues::EnemyRotationRate, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = DefaultValues::EnemyMovementSpeed;

	// Create health bar widget component
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(
		ComponentNames::HealthBarWidget);
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
	HealthBarWidget->SetRelativeLocation(
		FVector(0.0f, 0.0f, 100.0f));	   // Above the enemy
	HealthBarWidget->SetVisibility(false); // Hidden by default

	// AttributeComponent is now created in base class
}

void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();

	// AI Controller is now automatically spawned by the engine
	// No need to manually spawn it

	// Initialize the health bar widget
	InitializeHealthBar();

	// Update health bar with initial values
	UpdateHealthBar();
}

void AMyEnemy::InitializeHealthBar()
{
	// Set health bar widget class (set in Blueprint)
	if(HealthBarWidget && HealthBarWidgetClass)
	{
		HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
		HealthBarWidget->InitWidget();
	}
}

void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// AI logic moved to Behavior Tree for better performance
	// No longer handling movement and attack in Tick
}

void AMyEnemy::MoveTowardsPlayer(float DeltaTime)
{
	FVector Direction =
		PlayerCharacter->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f; // Keep on ground
	Direction.Normalize();

	// Simple move towards player
	AddMovementInput(Direction, 1.0f);
}

void AMyEnemy::AttackPlayer(ACharacter *Player)
{
	if(!Player)
		return;

	// If already attacking, don't start another attack
	if(bIsAttacking)
		return;

	float Distance =
		FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if(Distance <= AttackRange
		&& GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown)
	{
		LastAttackTime = GetWorld()->GetTimeSeconds();
		bIsAttacking = true;

		// Face the player before attacking
		FVector Direction = Player->GetActorLocation() - GetActorLocation();
		Direction.Z = 0.0f; // Keep on ground
		if(!Direction.IsNearlyZero())
		{
			FRotator NewRotation = Direction.Rotation();
			SetActorRotation(NewRotation);
		}

		// Play attack montage directly from code
		if(AttackMontage)
		{
			USkeletalMeshComponent *SkeletalMesh = GetMesh();
			if(SkeletalMesh)
			{
				UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
				if(AnimInstance)
				{
					AnimInstance->Montage_Stop(0.0f);
					// Clear previous binding to avoid multiple calls
					AnimInstance->OnMontageEnded.RemoveDynamic(
						this, &AMyEnemy::OnAttackMontageEnded);
					AnimInstance->Montage_Play(AttackMontage);
					// Bind to montage end to reset attack state
					AnimInstance->OnMontageEnded.AddDynamic(
						this, &AMyEnemy::OnAttackMontageEnded);
				}
			}
		}

		// Damage is applied via animation notify
	}
}

void AMyEnemy::ResetAttackState()
{
	bIsAttacking = false;
}

void AMyEnemy::OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
	if(Montage == AttackMontage)
	{
		ResetAttackState();
	}
}

UAbilitySystemComponent *AMyEnemy::GetAbilitySystem() const
{
	return AttributeComponent ? AttributeComponent->GetAbilitySystemComponent()
							  : nullptr;
}

void AMyEnemy::HandleDeath()
{
	SetActorTickEnabled(false);
	GetCharacterMovement()->DisableMovement();

	// Set dead state for animation blueprint
	bIsDead = true;

	// If no death animation, destroy after delay
	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimer, [this]() { Destroy(); }, DeathDelay, false);
}

void AMyEnemy::OnEnemyHealthChanged(float NewHealth)
{
	UpdateHealthBar();
}

float AMyEnemy::TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent,
	AController *EventInstigator, AActor *DamageCauser)
{
	// Apply damage to simple health
	Health = FMath::Max(0.0f, Health - DamageAmount);
	UpdateHealthBar();

	if(Health <= 0.0f)
	{
		// Death clears stun
		SetStunned(false);
		HandleDeath();
	}
	else
	{
		// Stun when taking damage but not dead
		SetStunned(true);
		// Reset stun after duration
		FTimerHandle StunTimer;
		GetWorld()->GetTimerManager().SetTimer(
			StunTimer, [this]() { SetStunned(false); }, StunDuration, false);
	}

	return DamageAmount;
}

void AMyEnemy::SetStunned(bool bStunned)
{
	bIsStunned = bStunned;

	// Disable movement while stunned
	GetCharacterMovement()->SetMovementMode(
		bIsStunned ? MOVE_None : MOVE_Walking);
}

void AMyEnemy::SetHealthBarVisible(bool bVisible)
{
	if(HealthBarWidget)
	{
		if(bVisible)
		{
			UpdateHealthBar();
		}
		HealthBarWidget->SetVisibility(bVisible);
	}
}

void AMyEnemy::SetFocused(bool bFocused)
{
	bIsFocused = bFocused;
	if(HealthBarWidget)
	{
		UpdateHealthBar();
	}
}

void AMyEnemy::InitializeDefaultAttributes()
{
	// Enemy uses simple health, no GAS needed
}
void AMyEnemy::UpdateHealthBar()
{
	if(!HealthBarWidget)
		return;
	UUserWidget *Widget = HealthBarWidget->GetUserWidgetObject();
	if(!Widget)
		return;
	// Assuming the widget has a ProgressBar named "HealthBar"
	UProgressBar *HealthProgressBar =
		Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar")));
	if(!HealthProgressBar)
	{
		return;
	}

	float HealthPercent = (MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f;

	HealthProgressBar->SetPercent(HealthPercent);

	// Set color based on focused state
	if(bIsFocused)
	{
		HealthProgressBar->SetFillColorAndOpacity(FLinearColor::Yellow);
	}
	else
	{
		HealthProgressBar->SetFillColorAndOpacity(FLinearColor::Red);
	}
}
