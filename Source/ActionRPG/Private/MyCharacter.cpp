// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Spring Arm Component
	SpringArmComponent =
		CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 350.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	SpringArmComponent->SocketOffset = FVector(0.f, 60.f, 70.f);

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.0f;

	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 10.0f;

	// Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
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

	InitializeAttributeComponent();
	InitializePlayerUI();
	SetupInputMapping();

	// Ensure AttributeComponent is ready before proceeding
	if(!IsAttributeSystemValid())
	{
		// Defer input setup if component not ready
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyCharacter::SetupPlayerInputDeferred);
	}
	else
	{
		// Setup input immediately if component is ready
		SetupPlayerInputComponent(this->InputComponent);
	}
}

void AMyCharacter::InitializeAttributeComponent()
{
	// Spawn the AttributeComponent from the selected class
	if(AttributeComponentClass)
	{
		if(!AttributeComponent)
		{
			// Create AttributeComponent with this character as the owner
			AttributeComponent =
				NewObject<UMyAttributeComponent>(this, AttributeComponentClass);
			if(AttributeComponent)
			{
				// Register the component with the character as owner
				AttributeComponent->RegisterComponent();
			}
		}
	}
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

void AMyCharacter::SetupInputMapping()
{
	// Input Mapping Context
	if(APlayerController *PlayerController =
			Cast<APlayerController>(Controller))
	{
		if(UEnhancedInputLocalPlayerSubsystem *Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
					PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(
	UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent *EnhancedInputComponent =
			CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
			MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(
			LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
			this, &AMyCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction,
			ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
		EnhancedInputComponent->BindAction(
			JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(
			DodgeAction, ETriggerEvent::Started, this, &AMyCharacter::Dodge);
		EnhancedInputComponent->BindAction(
			AttackAction, ETriggerEvent::Started, this, &AMyCharacter::Attack);
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
	if(IsAttacking())
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

	if(!IsAttributeSystemValid())
	{
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

	AttributeComponent->SetSprinting(true);
	ASC->AbilityLocalInputPressed(
		static_cast<int32>(EMyAbilityInputID::Sprint));
}

void AMyCharacter::StopSprint()
{
	if(IsAttacking())
		return;

	if(!IsAttributeSystemValid())
	{
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

	FGameplayTagContainer SprintAbilityTagContainer;
	SprintAbilityTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(FName("Ability.Sprint")));
	ASC->CancelAbilities(&SprintAbilityTagContainer);
	AttributeComponent->SetSprinting(false);
}

void AMyCharacter::Jump()
{
	if(IsAttacking())
		return;

	Super::Jump();

	if(!IsAttributeSystemValid())
	{
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

	ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Jump));
}

void AMyCharacter::StopJumping()
{
	if(IsAttacking())
		return;

	if(!IsAttributeSystemValid())
	{
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

	FGameplayTagContainer JumpAbilityTagContainer;
	JumpAbilityTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
	ASC->CancelAbilities(&JumpAbilityTagContainer);
}

void AMyCharacter::Dodge()
{
	if(IsAttacking())
		return;

	// Trigger ability system
	if(!IsAttributeSystemValid() || AttributeComponent->IsDodging())
	{
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

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
				if(UAbilitySystemComponent *ASCInner =
						AttributeComponent->GetAbilitySystemComponent())
				{
					FGameplayTagContainer DodgeAbilityTagContainer;
					DodgeAbilityTagContainer.AddTag(
						FGameplayTag::RequestGameplayTag(
							FName("Ability.Dodge")));
					ASCInner->CancelAbilities(&DodgeAbilityTagContainer);
				}
			}
		},
		DodgeCooldown, false);
}

void AMyCharacter::Attack()
{
	if(!IsAttributeSystemValid())
	{
		return;
	}

	UMyAttackAbility *ActiveAbility = GetActiveAttackAbility();
	if(ActiveAbility)
	{
		ActiveAbility->OnAttackInputPressed();
		return;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();

	ASC->AbilityLocalInputPressed(
		static_cast<int32>(EMyAbilityInputID::Attack));
}

void AMyCharacter::SetupPlayerInputDeferred()
{
	if(IsAttributeSystemValid())
	{
		if(UInputComponent *PlayerInputComp = this->InputComponent)
		{
			SetupPlayerInputComponent(PlayerInputComp);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyCharacter::SetupPlayerInputDeferred);
	}
}

void AMyCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);
	StopJumping();
}

void AMyCharacter::HandleDeath()
{
	bIsDead = true;

	// Disable input
	if(APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		PC->DisableInput(PC);
	}

	// Disable movement
	GetCharacterMovement()->DisableMovement();

	// Play death animation
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

		// Call base to destroy after delay
		if(AttributeComponent)
		{
			AttributeComponent->HandleDeath();
		}
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
	if(!IsAttributeSystemValid())
	{
		return nullptr;
	}

	UAbilitySystemComponent *ASC =
		AttributeComponent->GetAbilitySystemComponent();
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
