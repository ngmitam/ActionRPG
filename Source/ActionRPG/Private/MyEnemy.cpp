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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	// Create health bar widget component
	HealthBarWidget =
		CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
	HealthBarWidget->SetRelativeLocation(
		FVector(0.0f, 0.0f, 100.0f)); // Above the enemy

	// Create AttributeComponent properly
	AttributeComponent = CreateDefaultSubobject<UMyAttributeComponent>(
		TEXT("AttributeComponent"));
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

	// AttributeComponent is now created in constructor, just ensure it's ready
	if(AttributeComponent)
	{
		// Don't call InitializeAbilitySystem here - let AttributeComponent
		// handle its own timing
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyEnemy::InitializeDefaultAttributes);
	}
	else
	{
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

	float Distance =
		FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if(Distance <= AttackRange
		&& GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown
		&& !bIsAttacking)
	{
		LastAttackTime = GetWorld()->GetTimeSeconds();
		bIsAttacking = true;

		// Play attack animation if available
		if(AttackMontage)
		{
			USkeletalMeshComponent *SkeletalMesh = GetMesh();
			if(SkeletalMesh)
			{
				UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
				if(AnimInstance)
				{
					AnimInstance->Montage_Stop(0.0f);
					AnimInstance->Montage_Play(AttackMontage);
					// Bind to montage end
					AnimInstance->OnMontageEnded.AddDynamic(
						this, &AMyEnemy::OnAttackMontageEnded);
				}
			}
		}

		// Damage is now applied via animation notify
	}
}

UAbilitySystemComponent *AMyEnemy::GetAbilitySystem() const
{
	return AttributeComponent ? AttributeComponent->GetAbilitySystemComponent()
							  : nullptr;
}

void AMyEnemy::HandleDeath()
{
	// Disable tick
	SetActorTickEnabled(false);

	// Disable movement
	GetCharacterMovement()->DisableMovement();

	// Stop any abilities
	if(AttributeComponent && AttributeComponent->GetAbilitySystemComponent())
	{
		AttributeComponent->GetAbilitySystemComponent()->CancelAllAbilities();
	}

	// Play death animation
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

	// Fallback: destroy after delay
	if(AttributeComponent)
	{
		AttributeComponent->HandleDeath();
	}
}

void AMyEnemy::OnHealthChanged(float NewHealth)
{
	PreviousHealth = NewHealth;
	UpdateHealthBar();
}

void AMyEnemy::InitializeDefaultAttributes()
{
	if(AttributeComponent && AttributeComponent->GetAbilitySystemComponent()
		&& AttributeComponent->GetAbilitySystemComponent()
			   ->AbilityActorInfo.IsValid()
		&& AttributeComponent->GetAttributeSet())
	{
		// Bind to health change delegate now that we're sure GAS is fully
		// initialized
		AttributeComponent->GetOnHealthChanged().AddUObject(
			this, &AMyEnemy::OnHealthChanged);

		// Use the AttributeComponent's SetDefaultAttributes method instead of
		// direct access
		AttributeComponent->SetDefaultAttributes(
			DefaultHealth, DefaultMaxHealth, DefaultStamina, DefaultMaxStamina);

		// Update health bar after setting attributes
		UpdateHealthBar();
	}
	else
	{
		// Try again next frame if not ready
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyEnemy::InitializeDefaultAttributes);
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

void AMyEnemy::PlayMovementAnimation(bool bMoving)
{
	USkeletalMeshComponent *SkeletalMesh = GetMesh();
	if(!SkeletalMesh)
	{
		return;
	}

	UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
	if(!AnimInstance)
	{
		return;
	}

	// If attack montage is currently playing, don't interrupt
	UAnimMontage *Current = AnimInstance->GetCurrentActiveMontage();
	if(Current == AttackMontage)
	{
		return;
	}

	if(bMoving && WalkMontage)
	{
		if(!AnimInstance->Montage_IsPlaying(WalkMontage))
		{
			// Stop current montage if different
			if(CurrentMontage && CurrentMontage != WalkMontage)
			{
				AnimInstance->Montage_Stop(0.0f);
			}
			AnimInstance->Montage_Play(WalkMontage);
			CurrentMontage = WalkMontage;
		}
	}
	else if(!bMoving && IdleMontage)
	{
		if(!AnimInstance->Montage_IsPlaying(IdleMontage))
		{
			// Stop current montage if different
			if(CurrentMontage && CurrentMontage != IdleMontage)
			{
				AnimInstance->Montage_Stop(0.0f);
			}
			AnimInstance->Montage_Play(IdleMontage);
			CurrentMontage = IdleMontage;
		}
	}
	else
	{
		if(bMoving && !WalkMontage)
		{
		}
		if(!bMoving && !IdleMontage)
		{
		}
		// Stop any playing montage if not moving and no idle
		if(CurrentMontage)
		{
			AnimInstance->Montage_Stop(0.0f);
			CurrentMontage = nullptr;
		}
	}
}

void AMyEnemy::OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
	if(Montage == AttackMontage)
	{
		bIsAttacking = false;
		OnAttackFinished.Broadcast();
	}
}
