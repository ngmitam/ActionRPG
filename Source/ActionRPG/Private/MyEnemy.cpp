// Fill out your copyright notice in the Description page of Project Settings.

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

AMyEnemy::AMyEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

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
		FVector(0.0f, 0.0f, 100.0f)); // Above the enemy

	// AttributeComponent is now created in base class
}

void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Spawn AI Controller if specified
	if(AIControllerClass)
	{
		AAIController *NewController = GetWorld()->SpawnActor<AAIController>(
			AIControllerClass, GetActorLocation(), GetActorRotation());
		if(NewController)
		{
			NewController->Possess(this);
		}
	}

	// Initialize the health bar widget
	InitializeHealthBar();
}

void AMyEnemy::InitializeHealthBar()
{
	// Set health bar widget class (set in Blueprint)
	if(HealthBarWidget && HealthBarWidgetClass)
	{
		HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
	}

	// Don't bind to health change delegate here - wait until GAS is fully
	// initialized The binding will be done in InitializeDefaultAttributes when
	// we're sure everything is ready
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
	// Call base implementation first
	AMyBaseCharacter::HandleDeath();

	// Stop any abilities
	if(AttributeComponent && AttributeComponent->GetAbilitySystemComponent())
	{
		AttributeComponent->GetAbilitySystemComponent()->CancelAllAbilities();
	}

	// Play death animation with custom logic
	if(DeathMontage)
	{
		USkeletalMeshComponent *SkeletalMesh = GetMesh();
		if(SkeletalMesh)
		{
			UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Play(DeathMontage);
				// Set timer to destroy after animation
				float Duration =
					AnimInstance->Montage_GetPlayRate(DeathMontage) > 0.0f
						? DeathMontage->GetPlayLength()
							  / AnimInstance->Montage_GetPlayRate(DeathMontage)
						: DeathMontage->GetPlayLength();
				FTimerHandle DeathTimer;
				GetWorld()->GetTimerManager().SetTimer(
					DeathTimer,
					[this]()
					{
						if(AttributeComponent)
						{
							AttributeComponent->HandleDeath();
						}
					},
					Duration, false);
				return; // Don't destroy immediately
			}
		}
	}
}

void AMyEnemy::OnEnemyHealthChanged(float NewHealth)
{
	PreviousHealth = NewHealth;
	UpdateHealthBar();
}

void AMyEnemy::InitializeDefaultAttributes()
{
	// Call base implementation first
	AMyBaseCharacter::InitializeDefaultAttributes();

	// Additional enemy-specific initialization
	if(AttributeComponent && AttributeComponent->GetAbilitySystemComponent()
		&& AttributeComponent->GetAbilitySystemComponent()
			   ->AbilityActorInfo.IsValid()
		&& AttributeComponent->GetAttributeSet())
	{
		// Bind to health change delegate for enemy-specific logic
		AttributeComponent->GetOnHealthChanged().AddUObject(
			this, &AMyEnemy::OnEnemyHealthChanged);

		// Update health bar after setting attributes
		UpdateHealthBar();
	}
}
void AMyEnemy::UpdateHealthBar()
{
	if(HealthBarWidget && AttributeComponent)
	{
		UUserWidget *Widget = HealthBarWidget->GetUserWidgetObject();
		if(Widget)
		{
			// Assuming the widget has a ProgressBar named "HealthBar"
			UProgressBar *HealthProgressBar = Cast<UProgressBar>(
				Widget->GetWidgetFromName(TEXT("HealthBar")));
			if(HealthProgressBar)
			{
				float CurrentHealth = AttributeComponent->GetHealth();
				float MaxHealth = AttributeComponent->GetMaxHealth();
				float HealthPercent =
					(MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;

				HealthProgressBar->SetPercent(HealthPercent);
			}
		}
	}
}
