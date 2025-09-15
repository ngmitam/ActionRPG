// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyAttributeComponent.h"
#include "MyPlayerUI.h"
#include "Blueprint/UserWidget.h"

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

    // Attribute Component class set in Blueprint
    // Note: Component creation moved to BeginPlay() for proper initialization timing
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
    if (AttributeComponentClass)
    {

        if (!AttributeComponent)
        {
            // Create AttributeComponent with this character as the owner
            AttributeComponent = NewObject<UMyAttributeComponent>(this, AttributeComponentClass);
            if (AttributeComponent)
            {
                // Register the component with the character as owner
                AttributeComponent->RegisterComponent();
            }
        }
    }

    // Create and display the Player Controller
    if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
    {
        if (PlayerUIClass)
        {
            if (UUserWidget *Widget = CreateWidget<UUserWidget>(PlayerController, PlayerUIClass))
            {
                PlayerUIWidget = Cast<UMyPlayerUI>(Widget);
                if (PlayerUIWidget)
                {
                    PlayerUIWidget->SetOwningCharacter(this);
                    PlayerUIWidget->AddToViewport();
                }
            }
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

    // Ensure AttributeComponent is ready before proceeding
    if (!AttributeComponent || !AttributeComponent->GetAbilitySystemComponent() || !AttributeComponent->GetAbilitySystemComponent()->AbilityActorInfo.IsValid())
    {
        // Defer input setup if component not ready
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMyCharacter::SetupPlayerInputDeferred);
    }
    else
    {

        // Setup input immediately if component is ready
        SetupPlayerInputComponent(GetController()->GetPawn()->InputComponent);
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
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AMyCharacter::Dodge);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::Attack);
    }
}

void AMyCharacter::Move(const FInputActionValue &Value)
{
    if (IsAttacking())
        return;

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
    if (IsAttacking())
        return;

    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AMyCharacter::StartSprint()
{
    if (IsAttacking())
        return;

    if (!AttributeComponent)
    {
        return;
    }

    if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
    {

        // Check if AbilityActorInfo is valid before proceeding
        if (ASC->AbilityActorInfo.IsValid())
        {

            AttributeComponent->SetSprinting(true);
            ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Sprint));
        }
    }
}

void AMyCharacter::StopSprint()
{
    if (IsAttacking())
        return;

    if (!AttributeComponent)
    {
        return;
    }

    if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
    {

        // Check if AbilityActorInfo is valid before proceeding
        if (ASC->AbilityActorInfo.IsValid())
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
    if (IsAttacking())
        return;

    Super::Jump();

    if (!AttributeComponent)
    {
        return;
    }

    if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
    {

        // Check if AbilityActorInfo is valid before proceeding
        if (ASC->AbilityActorInfo.IsValid())
        {

            ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Jump));
        }
    }
}

void AMyCharacter::StopJumping()
{
    if (IsAttacking())
        return;

    if (!AttributeComponent)
    {
        return;
    }

    if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
    {

        // Check if AbilityActorInfo is valid before proceeding
        if (ASC->AbilityActorInfo.IsValid())
        {

            FGameplayTagContainer JumpAbilityTagContainer;
            JumpAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
            ASC->CancelAbilities(&JumpAbilityTagContainer);
        }
    }
}

void AMyCharacter::Dodge()
{
    if (IsAttacking())
        return;

    // Trigger ability system
    if (!AttributeComponent)
    {
        return;
    }

    if (UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent())
    {
        if (ASC->AbilityActorInfo.IsValid() && !AttributeComponent->IsDodging())
        {
            AttributeComponent->SetDodging(true);
            ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Dodge));

            // Reset dodge status after cooldown and cancel dodge ability
            FTimerHandle UnusedHandle;
            GetWorld()->GetTimerManager().SetTimer(
                UnusedHandle, [this]()
                {
                if (AttributeComponent)
                {
                    AttributeComponent->SetDodging(false);
                    if (UAbilitySystemComponent *ASCInner = AttributeComponent->GetAbilitySystemComponent())
                    {
                        FGameplayTagContainer DodgeAbilityTagContainer;
                        DodgeAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Dodge")));
                        ASCInner->CancelAbilities(&DodgeAbilityTagContainer);
                    }
                } },
                DodgeCooldown,
                false);
        }
    }
}

void AMyCharacter::Attack()
{
    if (!AttributeComponent || !AttributeComponent->GetAbilitySystemComponent() || !AttributeComponent->GetAbilitySystemComponent()->AbilityActorInfo.IsValid())
    {
        return;
    }

    UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent();

    TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();
    for (const FGameplayAbilitySpec &Spec : Abilities)
    {
        if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(UMyAttackAbility::StaticClass()))
        {
            if (Spec.IsActive())
            {
                UMyAttackAbility *AttackAbility = Cast<UMyAttackAbility>(Spec.GetPrimaryInstance());
                if (AttackAbility)
                {
                    AttackAbility->OnAttackInputPressed();
                    return;
                }
            }
            break;
        }
    }

    ASC->AbilityLocalInputPressed(static_cast<int32>(EMyAbilityInputID::Attack));
}

void AMyCharacter::SetupPlayerInputDeferred()
{
    if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent() && AttributeComponent->GetAbilitySystemComponent()->AbilityActorInfo.IsValid())
    {
        if (UInputComponent *PlayerInputComp = GetController()->GetPawn()->InputComponent)
        {
            SetupPlayerInputComponent(PlayerInputComp);
        }
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMyCharacter::SetupPlayerInputDeferred);
    }
}

void AMyCharacter::Landed(const FHitResult &Hit)
{
    Super::Landed(Hit);
    StopJumping();
}

bool AMyCharacter::IsAttacking() const
{
    if (!AttributeComponent || !AttributeComponent->GetAbilitySystemComponent())
    {
        return false;
    }

    UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent();
    TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();

    for (const FGameplayAbilitySpec &Spec : Abilities)
    {
        if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(UMyAttackAbility::StaticClass()))
        {
            return Spec.IsActive();
        }
    }

    return false;
}

int32 AMyCharacter::GetCurrentComboIndex() const
{
    if (!AttributeComponent || !AttributeComponent->GetAbilitySystemComponent())
    {
        return 0;
    }

    UAbilitySystemComponent *ASC = AttributeComponent->GetAbilitySystemComponent();
    TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();

    for (const FGameplayAbilitySpec &Spec : Abilities)
    {
        if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(UMyAttackAbility::StaticClass()) && Spec.IsActive())
        {
            UMyAttackAbility *AttackAbility = Cast<UMyAttackAbility>(Spec.GetPrimaryInstance());
            if (AttackAbility)
            {
                return AttackAbility->GetCurrentComboIndex();
            }
        }
    }

    return 0;
}
