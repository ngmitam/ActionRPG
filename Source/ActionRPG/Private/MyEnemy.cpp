// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEnemy.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"

AMyEnemy::AMyEnemy() {
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
  AttributeComponent =
      CreateDefaultSubobject<UMyAttributeComponent>(TEXT("AttributeComponent"));
}

void AMyEnemy::BeginPlay() {
  Super::BeginPlay();

  // Find player
  PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

  // AttributeComponent is now created in constructor, just ensure it's ready
  if (AttributeComponent) {
    // Don't call InitializeAbilitySystem here - let AttributeComponent handle
    // its own timing
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        this, &AMyEnemy::InitializeDefaultAttributes);
  } else {
  }

  // Initialize the health bar widget
  InitializeHealthBar();
}

void AMyEnemy::InitializeHealthBar() {
  // Set health bar widget class (set in Blueprint)
  if (HealthBarWidget && HealthBarWidgetClass) {
    HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
  }

  // Don't bind to health change delegate here - wait until GAS is fully
  // initialized The binding will be done in InitializeDefaultAttributes when
  // we're sure everything is ready
}

void AMyEnemy::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (PlayerCharacter) {
    MoveTowardsPlayer(DeltaTime);
    AttackPlayer();

    // Update movement animation state
    bool bCurrentlyMoving = GetVelocity().Size() > 0.1f;
    if (bCurrentlyMoving != bIsMoving) {
      bIsMoving = bCurrentlyMoving;
      PlayMovementAnimation(bIsMoving);
    }
  }
}

void AMyEnemy::MoveTowardsPlayer(float DeltaTime) {
  FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
  Direction.Z = 0.0f; // Keep on ground
  Direction.Normalize();

  // Simple move towards player
  AddMovementInput(Direction, 1.0f);
}

void AMyEnemy::AttackPlayer() {
  float Distance =
      FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
  if (Distance <= AttackRange &&
      GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown) {
    LastAttackTime = GetWorld()->GetTimeSeconds();

    // Play attack animation if available
    if (AttackMontage) {
      USkeletalMeshComponent *SkeletalMesh = GetMesh();
      if (SkeletalMesh) {
        UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
        if (AnimInstance) {
          AnimInstance->Montage_Stop(0.0f);
          AnimInstance->Montage_Play(AttackMontage);
        }
      }
    }

    // Apply damage to player
    if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent()) {
      AMyCharacter *Player = Cast<AMyCharacter>(PlayerCharacter);
      if (Player && Player->GetAttributeComponent() &&
          Player->GetAbilitySystem()) {
        // Create damage effect spec
        FGameplayEffectSpecHandle DamageSpecHandle =
            AttributeComponent->GetAbilitySystemComponent()->MakeOutgoingSpec(
                UMyDamageEffect::StaticClass(), 1.0f,
                FGameplayEffectContextHandle());
        if (DamageSpecHandle.IsValid()) {
          // Set damage magnitude (negative for damage)
          DamageSpecHandle.Data->SetSetByCallerMagnitude(
              FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
              -AttackDamage);

          // Apply to player
          AttributeComponent->GetAbilitySystemComponent()
              ->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
                                                Player->GetAbilitySystem());
        }
      }
    }
  }
}

UAbilitySystemComponent *AMyEnemy::GetAbilitySystem() const {
  return AttributeComponent ? AttributeComponent->GetAbilitySystemComponent()
                            : nullptr;
}

void AMyEnemy::HandleDeath() {
  // Disable tick
  SetActorTickEnabled(false);

  // Disable movement
  GetCharacterMovement()->DisableMovement();

  // Stop any abilities
  if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent()) {
    AttributeComponent->GetAbilitySystemComponent()->CancelAllAbilities();
  }

  // Play death animation
  if (DeathMontage) {
    USkeletalMeshComponent *SkeletalMesh = GetMesh();
    if (SkeletalMesh) {
      UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
      if (AnimInstance) {
        AnimInstance->Montage_Play(DeathMontage);
        // Set timer to destroy after animation
        float Duration =
            AnimInstance->Montage_GetPlayRate(DeathMontage) > 0.0f
                ? DeathMontage->GetPlayLength() /
                      AnimInstance->Montage_GetPlayRate(DeathMontage)
                : DeathMontage->GetPlayLength();
        FTimerHandle DeathTimer;
        GetWorld()->GetTimerManager().SetTimer(
            DeathTimer,
            [this]() {
              if (AttributeComponent) {
                AttributeComponent->HandleDeath();
              }
            },
            Duration, false);
        return; // Don't destroy immediately
      }
    }
  }

  // Fallback: destroy after delay
  if (AttributeComponent) {
    AttributeComponent->HandleDeath();
  }
}

void AMyEnemy::OnHealthChanged(float NewHealth) {
  PreviousHealth = NewHealth;
  UpdateHealthBar();
}

void AMyEnemy::InitializeDefaultAttributes() {
  if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent() &&
      AttributeComponent->GetAbilitySystemComponent()
          ->AbilityActorInfo.IsValid() &&
      AttributeComponent->GetAttributeSet()) {
    // Bind to health change delegate now that we're sure GAS is fully
    // initialized
    AttributeComponent->GetOnHealthChanged().AddUObject(
        this, &AMyEnemy::OnHealthChanged);

    // Use the AttributeComponent's SetDefaultAttributes method instead of
    // direct access
    AttributeComponent->SetDefaultAttributes(DefaultHealth, DefaultMaxHealth,
                                             DefaultStamina, DefaultMaxStamina);

    // Update health bar after setting attributes
    UpdateHealthBar();
  } else {
    // Try again next frame if not ready
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        this, &AMyEnemy::InitializeDefaultAttributes);
  }
}
void AMyEnemy::UpdateHealthBar() {
  if (HealthBarWidget && AttributeComponent) {
    UUserWidget *Widget = HealthBarWidget->GetUserWidgetObject();
    if (Widget) {
      // Assuming the widget has a ProgressBar named "HealthBar"
      UProgressBar *HealthProgressBar =
          Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar")));
      if (HealthProgressBar) {
        float CurrentHealth = AttributeComponent->GetHealth();
        float MaxHealth = AttributeComponent->GetMaxHealth();
        float HealthPercent =
            (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;

        HealthProgressBar->SetPercent(HealthPercent);
      }
    }
  }
}

void AMyEnemy::PlayMovementAnimation(bool bMoving) {
  UAnimMontage *DesiredMontage = nullptr;

  if (bMoving && WalkMontage) {
    DesiredMontage = WalkMontage;
  } else if (!bMoving && IdleMontage) {
    DesiredMontage = IdleMontage;
  }

  // Only change if different from current
  if (DesiredMontage != CurrentMontage) {
    USkeletalMeshComponent *SkeletalMesh = GetMesh();
    if (SkeletalMesh) {
      UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
      if (AnimInstance) {
        // Stop current montage
        if (CurrentMontage) {
          AnimInstance->Montage_Stop(0.0f);
        }

        // Play new montage if available
        if (DesiredMontage) {
          AnimInstance->Montage_Play(DesiredMontage);
        }

        CurrentMontage = DesiredMontage;
      }
    }
  }
}
