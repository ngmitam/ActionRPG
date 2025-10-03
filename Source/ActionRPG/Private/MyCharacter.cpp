// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyAttributeComponent.h"
#include "MyEnemy.h"
#include "MyPlayerUI.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetCharacterMovement()->RotationRate =
		FRotator(0.0f, DefaultValues::CharacterRotationRate, 0.0f);

	// Spring Arm Component
	SpringArmComponent =
		CreateDefaultSubobject<USpringArmComponent>(ComponentNames::SpringArm);
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = DefaultValues::SpringArmLength;
	SpringArmComponent->bUsePawnControlRotation = true;

	SpringArmComponent->SocketOffset = FVector(
		DefaultValues::CameraSocketOffsetX, DefaultValues::CameraSocketOffsetY,
		DefaultValues::CameraSocketOffsetZ);

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = DefaultValues::CameraLagSpeed;

	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = DefaultValues::CameraLagSpeed;

	// Camera Component
	CameraComponent =
		CreateDefaultSubobject<UCameraComponent>(ComponentNames::Camera);
	CameraComponent->SetupAttachment(
		SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	// Attribute Component will be created in BeginPlay
}
void AMyCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
}

void AMyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializePlayerUI();

	// Start updating nearby enemies
	UpdateNearbyEnemies(); // Initial update
	GetWorld()->GetTimerManager().SetTimer(UpdateEnemiesTimerHandle, this,
		&AMyCharacter::UpdateNearbyEnemies, DefaultValues::EnemyUpdateInterval,
		true);
}

void AMyCharacter::InitializePlayerUI()
{
	// Create and display the Player Controller
	if(APlayerController *PlayerController =
			Cast<APlayerController>(Controller))
	{
		if(PlayerUIClass)
		{
			if(UUserWidget *Widget =
					CreateWidget<UUserWidget>(PlayerController, PlayerUIClass))
			{
				PlayerUIWidget = Cast<UMyPlayerUI>(Widget);
				if(PlayerUIWidget)
				{
					PlayerUIWidget->SetOwningCharacter(this);
					PlayerUIWidget->AddToViewport();
				}
			}
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update camera lock
	if(bCameraLocked && CurrentTarget)
	{
		FVector Direction =
			(CurrentTarget->GetActorLocation() - GetActorLocation())
				.GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		if(Controller)
		{
			Controller->SetControlRotation(TargetRotation);
		}
	}
}

void AMyCharacter::Move(const FInputActionValue &Value)
{
	if(IsAttacking())
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if(Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyCharacter::Look(const FInputActionValue &Value)
{
	if(IsAttacking() || bCameraLocked)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyCharacter::StartSprint()
{
	if(IsAttacking())
		return;

	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return;
	}

	AttributeComponent->SetSprinting(true);
	ASC->AbilityLocalInputPressed(
		static_cast<int32>(EMyAbilityInputID::Sprint));
}

void AMyCharacter::StopSprint()
{
	if(IsAttacking())
		return;

	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return;
	}

	FGameplayTagContainer SprintAbilityTagContainer;
	SprintAbilityTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(AbilityTags::Sprint));
	ASC->CancelAbilities(&SprintAbilityTagContainer);
	AttributeComponent->SetSprinting(false);
}

void AMyCharacter::Jump()
{
	if(IsAttacking())
		return;

	Super::Jump();

	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return;
	}

	ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Jump));
}

void AMyCharacter::StopJumping()
{
	if(IsAttacking())
		return;

	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return;
	}

	FGameplayTagContainer JumpAbilityTagContainer;
	JumpAbilityTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(AbilityTags::Jump));
	ASC->CancelAbilities(&JumpAbilityTagContainer);
}

void AMyCharacter::Dodge()
{
	if(IsAttacking())
		return;

	// Trigger ability system
	if(!IsAbilitySystemReady() || AttributeComponent->IsDodging())
	{
		return;
	}

	UAbilitySystemComponent *ASC = GetAbilitySystemComponent();

	AttributeComponent->SetDodging(true);
	ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Dodge));

	// Reset dodge status after cooldown and cancel dodge ability
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(
		UnusedHandle,
		[this]()
		{
			if(AttributeComponent)
			{
				AttributeComponent->SetDodging(false);
				if(UAbilitySystemComponent *ASCInner = TryGetAbilitySystem())
				{
					FGameplayTagContainer DodgeAbilityTagContainer;
					DodgeAbilityTagContainer.AddTag(
						FGameplayTag::RequestGameplayTag(AbilityTags::Dodge));
					ASCInner->CancelAbilities(&DodgeAbilityTagContainer);
				}
			}
		},
		DefaultValues::DodgeCooldown, false);
}

void AMyCharacter::Attack()
{
	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return;
	}

	UMyAttackAbility *ActiveAbility = GetActiveAttackAbility();
	if(ActiveAbility)
	{
		ActiveAbility->OnAttackInputPressed();
		return;
	}

	// Check if attack ability is available
	TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();
	for(const FGameplayAbilitySpec &Spec : Abilities)
	{
		if(Spec.Ability
			&& Spec.Ability->GetClass()->IsChildOf(
				UMyAttackAbility::StaticClass()))
		{
			ASC->AbilityLocalInputPressed(
				static_cast<int32>(EMyAbilityInputID::Attack));
			break;
		}
	}
}

void AMyCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);
	StopJumping();
}

void AMyCharacter::HandleDeath()
{
	// Call base implementation first
	AMyBaseCharacter::HandleDeath();

	// Disable input
	if(APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		PC->DisableInput(PC);
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
				float Duration = AnimInstance->Montage_Play(DeathMontage);
				// Set timer to destroy after animation
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
			}
		}
	}
	else
	{
		// Fallback: ragdoll
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

bool AMyCharacter::IsAttacking() const
{
	return GetActiveAttackAbility() != nullptr;
}

int32 AMyCharacter::GetCurrentComboIndex() const
{
	UMyAttackAbility *Ability = GetActiveAttackAbility();
	return Ability ? Ability->GetCurrentComboIndex() : 0;
}

UMyAttackAbility *AMyCharacter::GetActiveAttackAbility() const
{
	UAbilitySystemComponent *ASC = TryGetAbilitySystem();
	if(!ASC)
	{
		return nullptr;
	}

	TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();

	for(const FGameplayAbilitySpec &Spec : Abilities)
	{
		if(Spec.Ability
			&& Spec.Ability->GetClass()->IsChildOf(
				UMyAttackAbility::StaticClass()))
		{
			if(Spec.IsActive())
			{
				return Cast<UMyAttackAbility>(Spec.GetPrimaryInstance());
			}
		}
	}

	return nullptr;
}

void AMyCharacter::UpdateNearbyEnemies()
{
	PreviousNearbyEnemies = NearbyEnemies;
	NearbyEnemies.Empty();

	FindNearbyEnemies();
	SortEnemiesByDistance();
	UpdateHealthBarVisibility();
	ValidateCurrentTarget();
}

void AMyCharacter::FindNearbyEnemies()
{
	TArray<AActor *> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(), AMyEnemy::StaticClass(), AllEnemies);

	for(AActor *Actor : AllEnemies)
	{
		if(AMyEnemy *Enemy = Cast<AMyEnemy>(Actor))
		{
			float Distance =
				FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
			if(Distance <= DefaultValues::EnemyDetectionRange)
			{
				NearbyEnemies.Add(Enemy);
			}
		}
	}
}

void AMyCharacter::SortEnemiesByDistance()
{
	NearbyEnemies.Sort(
		[this](const AMyEnemy &A, const AMyEnemy &B)
		{
			float DistA =
				FVector::DistSquared(GetActorLocation(), A.GetActorLocation());
			float DistB =
				FVector::DistSquared(GetActorLocation(), B.GetActorLocation());
			return DistA < DistB;
		});
}

void AMyCharacter::UpdateHealthBarVisibility()
{
	// Hide health bars for enemies that are no longer nearby
	for(AMyEnemy *Enemy : PreviousNearbyEnemies)
	{
		if(!NearbyEnemies.Contains(Enemy) && Enemy != CurrentTarget)
		{
			Enemy->SetHealthBarVisible(false);
		}
	}

	// Show health bars for nearby enemies
	for(AMyEnemy *Enemy : NearbyEnemies)
	{
		if(Enemy != CurrentTarget)
		{
			Enemy->SetHealthBarVisible(true);
		}
	}
}

void AMyCharacter::ValidateCurrentTarget()
{
	if(CurrentTarget
		&& (!NearbyEnemies.Contains(CurrentTarget) || CurrentTarget->bIsDead))
	{
		CycleTarget();
	}
}
void AMyCharacter::FocusEnemy()
{
	CycleTarget();
}

void AMyCharacter::CycleTarget()
{
	AMyEnemy *OldTarget = CurrentTarget;
	if(NearbyEnemies.Num() == 0)
	{
		HandleNoNearbyEnemies(OldTarget);
		return;
	}

	// Create list of all possible targets including no target
	TArray<AMyEnemy *> AllTargets = CreateTargetList();

	int32 CurrentIndex = FindCurrentTargetIndex(AllTargets);
	AMyEnemy *NewTarget = GetNextTarget(AllTargets, CurrentIndex);

	CurrentTarget = NewTarget;

	UpdateCameraLock(NewTarget);
	UpdateTargetVisibilityAndFocus(OldTarget, NewTarget);
}

void AMyCharacter::HandleNoNearbyEnemies(AMyEnemy *OldTarget)
{
	CurrentTarget = nullptr;
	bCameraLocked = false;
	if(OldTarget)
	{
		if(!NearbyEnemies.Contains(OldTarget))
		{
			OldTarget->SetHealthBarVisible(false);
			OldTarget->SetFocused(false);
		}
	}
}

TArray<AMyEnemy *> AMyCharacter::CreateTargetList() const
{
	TArray<AMyEnemy *> AllTargets;
	AllTargets.Add(nullptr); // No target option
	for(AMyEnemy *Enemy : NearbyEnemies)
	{
		AllTargets.Add(Enemy);
	}
	return AllTargets;
}

int32 AMyCharacter::FindCurrentTargetIndex(
	const TArray<AMyEnemy *> &AllTargets) const
{
	int32 CurrentIndex = AllTargets.Find(CurrentTarget);
	if(CurrentIndex == INDEX_NONE)
	{
		return 0; // Default to first target (no target)
	}
	return CurrentIndex;
}

AMyEnemy *AMyCharacter::GetNextTarget(
	const TArray<AMyEnemy *> &AllTargets, int32 CurrentIndex) const
{
	int32 NextIndex = (CurrentIndex + 1) % AllTargets.Num();
	return AllTargets[NextIndex];
}

void AMyCharacter::UpdateCameraLock(AMyEnemy *NewTarget)
{
	bCameraLocked = (NewTarget != nullptr);
}

void AMyCharacter::UpdateTargetVisibilityAndFocus(
	AMyEnemy *OldTarget, AMyEnemy *NewTarget)
{
	// Update old target
	if(OldTarget && OldTarget != NewTarget)
	{
		if(!NearbyEnemies.Contains(OldTarget))
		{
			OldTarget->SetHealthBarVisible(false);
			OldTarget->SetFocused(false);
		}
		else
		{
			OldTarget->SetFocused(false);
		}
	}

	// Update new target
	if(NewTarget)
	{
		NewTarget->SetHealthBarVisible(true);
		NewTarget->SetFocused(true);
	}
}
