// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyBaseCharacter.h"

#include "MyAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyBaseCharacter::AMyBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// AttributeComponent will be created dynamically in BeginPlay to allow
	// Blueprint subclasses
}

void AMyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Create AttributeComponent dynamically to allow Blueprint subclasses
	if(!AttributeComponent)
	{
		UClass *ComponentClass = AttributeComponentClass.Get();
		if(!ComponentClass)
		{
			ComponentClass = UMyAttributeComponent::StaticClass();
		}
		AttributeComponent =
			NewObject<UMyAttributeComponent>(this, ComponentClass);
		if(AttributeComponent)
		{
			AttributeComponent->RegisterComponent();
		}
	}

	// Initialize attribute component
	InitializeAttributeComponent();
}

void AMyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyBaseCharacter::HandleDeath()
{
	// Base implementation - disable movement and set death status
	if(UCharacterMovementComponent *MovementComp = GetCharacterMovement())
	{
		MovementComp->DisableMovement();
	}

	bIsDead = true;

	// Play death animation if available
	if(DeathMontage)
	{
		USkeletalMeshComponent *SkeletalMesh = GetMesh();
		if(SkeletalMesh)
		{
			UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Play(DeathMontage);
			}
		}
	}

	// Call attribute component's death handling
	if(AttributeComponent)
	{
		AttributeComponent->HandleDeath();
	}
}

float AMyBaseCharacter::TakeDamage(float DamageAmount,
	struct FDamageEvent const &DamageEvent, class AController *EventInstigator,
	AActor *DamageCauser)
{
	float ActualDamage = Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Apply damage through GAS attributes
	if(AttributeComponent && AttributeComponent->IsAbilitySystemValid())
	{
		// Apply damage to health attribute
		AttributeComponent->GetAbilitySystemComponent()->ApplyModToAttribute(
			AttributeComponent->GetAttributeSet()->GetHealthAttribute(),
			EGameplayModOp::Additive, -ActualDamage);
	}

	return ActualDamage;
}

void AMyBaseCharacter::OnHealthChanged(float NewHealth)
{
	// Base implementation - can be overridden by subclasses
	if(NewHealth <= 0.0f && !bIsDead)
	{
		HandleDeath();
	}
}

void AMyBaseCharacter::InitializeDefaultAttributes()
{
	if(!IsAttributeSystemValid())
	{
		// Try again next frame if not ready
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyBaseCharacter::InitializeDefaultAttributes);
		return;
	}

	// Only initialize if not already done
	if(bAttributesInitialized)
	{
		return;
	}

	// Bind to health change delegate
	AttributeComponent->GetOnHealthChanged().AddUObject(
		this, &AMyBaseCharacter::OnHealthChanged);

	// Set default attributes using struct
	FDefaultAttributes DefaultAttrs;

	// Use blueprint-set values if they differ from class defaults, otherwise
	// use defaults
	DefaultAttrs.Health = (DefaultHealth != DefaultValues::DefaultHealth)
							  ? DefaultHealth
							  : DefaultValues::DefaultHealth;
	DefaultAttrs.MaxHealth =
		(DefaultMaxHealth != DefaultValues::DefaultMaxHealth)
			? DefaultMaxHealth
			: DefaultValues::DefaultMaxHealth;
	DefaultAttrs.Stamina = (DefaultStamina != DefaultValues::DefaultStamina)
							   ? DefaultStamina
							   : DefaultValues::DefaultStamina;
	DefaultAttrs.MaxStamina =
		(DefaultMaxStamina != DefaultValues::DefaultMaxStamina)
			? DefaultMaxStamina
			: DefaultValues::DefaultMaxStamina;
	DefaultAttrs.MaxWalkSpeed = AttributeComponent->GetDefaultMaxWalkSpeed();

	AttributeComponent->SetDefaultAttributes(DefaultAttrs);

	// Mark as initialized
	bAttributesInitialized = true;
}

void AMyBaseCharacter::InitializeAttributeComponent()
{
	// Initialize attribute component if it exists
	if(AttributeComponent)
	{
		// Defer initialization to ensure proper timing
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyBaseCharacter::InitializeDefaultAttributes);
	}
}
