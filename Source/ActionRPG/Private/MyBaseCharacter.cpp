// Fill out your copyright notice in the Description page of Project Settings.

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
	SetActorTickEnabled(false);
	GetCharacterMovement()->DisableMovement();

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
	if(AttributeComponent && AttributeComponent->GetAbilitySystemComponent()
		&& AttributeComponent->GetAbilitySystemComponent()
			   ->AbilityActorInfo.IsValid()
		&& AttributeComponent->GetAttributeSet())
	{
		// Bind to health change delegate
		AttributeComponent->GetOnHealthChanged().AddUObject(
			this, &AMyBaseCharacter::OnHealthChanged);

		// Set default attributes using struct
		FDefaultAttributes DefaultAttrs;
		DefaultAttrs.Health = DefaultHealth;
		DefaultAttrs.MaxHealth = DefaultMaxHealth;
		DefaultAttrs.Stamina = DefaultStamina;
		DefaultAttrs.MaxStamina = DefaultMaxStamina;
		DefaultAttrs.BaseDamage = AttributeComponent->GetDefaultBaseDamage();
		DefaultAttrs.MaxWalkSpeed =
			AttributeComponent->GetDefaultMaxWalkSpeed();

		AttributeComponent->SetDefaultAttributes(DefaultAttrs);
	}
	else
	{
		// Try again next frame if not ready
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &AMyBaseCharacter::InitializeDefaultAttributes);
	}
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
