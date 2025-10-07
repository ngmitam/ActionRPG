// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyAbilityTypes.h"
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
		FRotator(0.0f, FGameConfig::GetDefault().CharacterRotationRate, 0.0f);

	// Spring Arm Component
	SpringArmComponent =
		CreateDefaultSubobject<USpringArmComponent>(ComponentNames::SpringArm);
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength =
		FGameConfig::GetDefault().SpringArmLength;
	SpringArmComponent->bUsePawnControlRotation = true;

	SpringArmComponent->SocketOffset =
		FVector(FGameConfig::GetDefault().CameraSocketOffsetX,
			FGameConfig::GetDefault().CameraSocketOffsetY,
			FGameConfig::GetDefault().CameraSocketOffsetZ);

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed =
		FGameConfig::GetDefault().CameraLagSpeed;

	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed =
		FGameConfig::GetDefault().CameraLagSpeed;

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
		&AMyCharacter::UpdateNearbyEnemies,
		FGameConfig::GetDefault().EnemyUpdateInterval, true);
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
	if(!CanPerformAbility())
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
	if(!CanPerformAbility())
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
	if(!CanPerformAbility())
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
	if(!CanPerformAbility())
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
	if(!CanPerformAbility() || AttributeComponent->IsDodging())
	{
		return;
	}

	UAbilitySystemComponent *ASC = GetAbilitySystemComponent();

	AttributeComponent->SetDodging(true);
	ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Dodge));

	// Reset dodge status after cooldown
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this,
		&AMyCharacter::ResetDodgeStatus,
		FGameConfig::GetDefault().DodgeCooldown, false);
}

void AMyCharacter::ResetDodgeStatus()
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

	// Reset the level after a delay
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this,
		&AMyCharacter::ResetLevel, FGameConfig::GetDefault().DeathResetDelay,
		false);
}

void AMyCharacter::ResetLevel()
{
	// Get current level name
	FString CurrentLevelName = GetWorld()->GetMapName();
	// Remove any path prefix if present
	int32 LastSlashIndex;
	if(CurrentLevelName.FindLastChar('/', LastSlashIndex))
	{
		CurrentLevelName =
			CurrentLevelName.Right(CurrentLevelName.Len() - LastSlashIndex - 1);
	}
	// Remove extension
	CurrentLevelName = CurrentLevelName.Replace(TEXT(".umap"), TEXT(""));

	// Reload the current level
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
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

	const float DetectionRange = FGameConfig::GetDefault().EnemyDetectionRange;
	const FVector PlayerLocation = GetActorLocation();

	for(AActor *Actor : AllEnemies)
	{
		if(AMyEnemy *Enemy = Cast<AMyEnemy>(Actor))
		{
			const float Distance =
				FVector::Dist(PlayerLocation, Enemy->GetActorLocation());
			if(Distance <= DetectionRange)
			{
				NearbyEnemies.Add(Enemy);
			}
		}
	}
}

void AMyCharacter::SortEnemiesByDistance()
{
	const FVector PlayerLocation = GetActorLocation();

	NearbyEnemies.Sort(
		[PlayerLocation](const AMyEnemy &A, const AMyEnemy &B)
		{
			const float DistA =
				FVector::DistSquared(PlayerLocation, A.GetActorLocation());
			const float DistB =
				FVector::DistSquared(PlayerLocation, B.GetActorLocation());
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
		ClearTarget();

		// Focus new nearest enemy if available
		if(NearbyEnemies.Num() > 0)
		{
			SetTarget(NearbyEnemies[0]);
		}
	}
}

void AMyCharacter::SetTarget(AMyEnemy *NewTarget)
{
	if(CurrentTarget == NewTarget)
	{
		return;
	}

	// Clear current target if different
	if(CurrentTarget)
	{
		ClearTarget();
	}

	// Set new target
	CurrentTarget = NewTarget;
	if(CurrentTarget)
	{
		bCameraLocked = true;
		CurrentTarget->SetFocused(true);
		CurrentTarget->SetHealthBarVisible(true);
	}
	else
	{
		bCameraLocked = false;
	}
}

void AMyCharacter::ClearTarget()
{
	if(CurrentTarget)
	{
		AMyEnemy *OldTarget = CurrentTarget;
		CurrentTarget = nullptr;
		bCameraLocked = false;

		OldTarget->SetFocused(false);
		if(!NearbyEnemies.Contains(OldTarget))
		{
			OldTarget->SetHealthBarVisible(false);
		}
	}
}

void AMyCharacter::FocusEnemy()
{
	if(CurrentTarget)
	{
		ClearTarget();
	}
	else
	{
		// Focus nearest enemy
		if(NearbyEnemies.Num() > 0)
		{
			SetTarget(NearbyEnemies[0]);
		}
	}
}

void AMyCharacter::CycleTarget()
{
	// If no nearby enemies, clear target
	if(NearbyEnemies.Num() == 0)
	{
		ClearTarget();
		return;
	}

	// Find current target index in nearby enemies
	int32 CurrentIndex = NearbyEnemies.Find(CurrentTarget);
	if(CurrentIndex == INDEX_NONE)
	{
		// Current target not in nearby enemies, start with first enemy
		CurrentIndex = 0;
	}

	// Cycle to next enemy
	const int32 NextIndex = (CurrentIndex + 1) % NearbyEnemies.Num();
	AMyEnemy *NewTarget = NearbyEnemies[NextIndex];

	SetTarget(NewTarget);
}
