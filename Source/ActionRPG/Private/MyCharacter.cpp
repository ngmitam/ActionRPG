// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyAttributeComponent.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;

    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    // Spring Arm Component
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
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
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;
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

    // Spawn the AttributeComponent from the selected class
    if (AttributeComponentClass && !AttributeComponent)
    {
        AttributeComponent = NewObject<UMyAttributeComponent>(this, AttributeComponentClass);
        if (AttributeComponent)
        {
            AttributeComponent->RegisterComponent();
        }
    }

    // Input Mapping Context
    if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyCharacter::StartSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    }
}

void AMyCharacter::Move(const FInputActionValue &Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AMyCharacter::Look(const FInputActionValue &Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AMyCharacter::StartSprint()
{
    if (AttributeComponent)
    {
        if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
        {
            AttributeComponent->SetSprinting(true);
            ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Sprint));
        }
    }
}

void AMyCharacter::StopSprint()
{
    if (AttributeComponent)
    {
        if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
        {
            FGameplayTagContainer SprintAbilityTagContainer;
            SprintAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sprint")));
            ASC->CancelAbilities(&SprintAbilityTagContainer);
            AttributeComponent->SetSprinting(false);
        }
    }
}

void AMyCharacter::Jump()
{
    Super::Jump();

    if (AttributeComponent)
    {
        if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
        {
            ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Jump));
        }
    }
}

void AMyCharacter::StopJumping()
{
    if (AttributeComponent)
    {
        if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
        {
            FGameplayTagContainer JumpAbilityTagContainer;
            JumpAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
            ASC->CancelAbilities(&JumpAbilityTagContainer);
        }
    }
}

void AMyCharacter::Landed(const FHitResult &Hit)
{
    Super::Landed(Hit);
    StopJumping();
}
