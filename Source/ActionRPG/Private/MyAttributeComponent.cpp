// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeComponent.h"

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectTypes.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"

UMyAttributeComponent::UMyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(
		"AbilitySystemComponent");
}

UAbilitySystemComponent *
UMyAttributeComponent::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void UMyAttributeComponent::OnRegister()
{
	Super::OnRegister();

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterComponent();
	}
}

void UMyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Always defer GAS initialization to ensure proper timing
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		this, &UMyAttributeComponent::DeferredInitialize);
}

void UMyAttributeComponent::DeferredInitialize()
{
	// Verify that we have a valid owner (should be the character)
	if(!ValidateOwner())
	{
		// If no owner, try again next frame
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &UMyAttributeComponent::DeferredInitialize);
		return;
	}

	// Ensure AbilitySystemComponent exists and is registered
	if(!AbilitySystemComponent || !AbilitySystemComponent->IsRegistered())
	{
		// Component not created or registered, try again next frame
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &UMyAttributeComponent::DeferredInitialize);
		return;
	}

	// Initialize AbilityActorInfo if not already done
	if(!AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
	}

	// Initialize GAS components
	InitializeGASComponents();

	// Double-check that AbilityActorInfo is now valid
	if(AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		InitializeAbilitySystem();
	}
	else
	{
		// Still not ready, try again next frame (with a limit to prevent
		// infinite loops)
		static int32 RetryCount = 0;
		if(RetryCount < MaxInitializationRetries)
		{
			RetryCount++;
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				this, &UMyAttributeComponent::DeferredInitialize);
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("Failed to initialize Ability System after %d retries"),
				MaxInitializationRetries);
		}
	}
}

bool UMyAttributeComponent::ValidateOwner() const
{
	AActor *Owner = GetOwner();
	if(!Owner)
	{
		return false;
	}

	// Verify that the owner is properly initialized
	return Owner->IsActorInitialized();
}

void UMyAttributeComponent::InitializeGASComponents()
{
	// Create AttributeSet with Owner as outer
	if(!AttributeSet)
	{
		AttributeSet = NewObject<UMyAttributeSet>(GetOwner());
	}

	// Add AttributeSet to AbilitySystemComponent if not already added
	if(AttributeSet
		&& !AbilitySystemComponent->HasAttributeSetForAttribute(
			AttributeSet->GetHealthAttribute()))
	{
		AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);
	}

	// Set replication properties after AbilityActorInfo is initialized
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(
		EGameplayEffectReplicationMode::Minimal);
}

void UMyAttributeComponent::InitializeAbilitySystem()
{
	// Final validation before initializing
	if(!IsAbilitySystemValid() || !GetOwner())
	{
		return;
	}

	// At this point, we know AbilitySystemComponent and AbilityActorInfo are
	// valid and the owner is initialized, so we can safely proceed

	// Now it's safe to initialize attributes and abilities
	InitializeAttributes();
	GiveDefaultAbilities();

	// Subscribe to attribute change callbacks
	if(AttributeSet)
	{
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(
				AttributeSet->GetStaminaAttribute())
			.AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(
				AttributeSet->GetMaxWalkSpeedAttribute())
			.AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(
				AttributeSet->GetHealthAttribute())
			.AddUObject(this, &UMyAttributeComponent::OnAttributeChange);

		// If owner is a character, set initial walk speed
		if(ACharacter *Character = Cast<ACharacter>(GetOwner()))
		{
			Character->GetCharacterMovement()->MaxWalkSpeed =
				AttributeSet->GetMaxWalkSpeed();
		}
	}
}

void UMyAttributeComponent::InitializeAttributes()
{
	if(!IsAbilitySystemValid())
	{
		return;
	}

	// Use the SetDefaultAttributes method to set the base values
	SetDefaultAttributes(
		DefaultHealth, DefaultMaxHealth, DefaultStamina, DefaultMaxStamina);
}

void UMyAttributeComponent::GiveDefaultAbilities()
{
	if(!IsAbilitySystemValid() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for(int32 i = 0; i < DefaultAbilityClasses.Num(); ++i)
	{
		TSubclassOf<UMyGameplayAbility> Ability = DefaultAbilityClasses[i];

		if(!Ability)
		{
			continue;
		}

		// Validate the ability class
		UMyGameplayAbility *AbilityCDO = Ability.GetDefaultObject();
		if(!AbilityCDO)
		{
			continue;
		}

		// Validate AbilityInputID
		int32 InputID = static_cast<int32>(AbilityCDO->AbilityInputID);
		if(InputID < 0)
		{
			continue;
		}

		// Safely give the ability
		FGameplayAbilitySpec AbilitySpec(Ability, 1, InputID, GetOwner());
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

float UMyAttributeComponent::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float UMyAttributeComponent::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

float UMyAttributeComponent::GetStamina() const
{
	return AttributeSet ? AttributeSet->GetStamina() : 0.0f;
}

float UMyAttributeComponent::GetMaxStamina() const
{
	return AttributeSet ? AttributeSet->GetMaxStamina() : 0.0f;
}

float UMyAttributeComponent::GetBaseDamage() const
{
	return AttributeSet ? AttributeSet->GetBaseDamage() : 0.0f;
}

float UMyAttributeComponent::GetMaxWalkSpeed() const
{
	return AttributeSet ? AttributeSet->GetMaxWalkSpeed() : 0.0f;
}

void UMyAttributeComponent::OnAttributeChange(
	const FOnAttributeChangeData &Data)
{
	if(Data.Attribute == AttributeSet->GetMaxWalkSpeedAttribute())
	{
		if(ACharacter *Character = Cast<ACharacter>(GetOwner()))
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
		}
	}
	else if(Data.Attribute == AttributeSet->GetStaminaAttribute())
	{
		OnStaminaChange(Data);
	}
	else if(Data.Attribute == AttributeSet->GetHealthAttribute())
	{
		// Broadcast health change to listeners
		OnHealthChanged.Broadcast(Data.NewValue);

		// Handle health changes if needed, e.g., update UI or trigger events
		if(Data.NewValue <= 0.0f)
		{
			HandleDeath();
		}
	}
}

void UMyAttributeComponent::OnStaminaChange(const FOnAttributeChangeData &Data)
{
	if(Data.NewValue <= 0.0f && AbilitySystemComponent
		&& AbilitySystemComponent->AbilityActorInfo.IsValid()
		&& AbilitySystemComponent->HasMatchingGameplayTag(
			FGameplayTag::RequestGameplayTag(FName("State.Sprinting"))))
	{
		// Out of stamina - stop sprinting
		SetSprinting(false);
		// Note: The actual ability cancellation should be handled by the
		// ability system or character
	}
}

void UMyAttributeComponent::HandleDeath()
{
	AActor *Owner = GetOwner();
	if(Owner)
	{
		// Default: destroy actor after a delay
		Owner->SetLifeSpan(2.0f); // Destroy after 2 seconds
	}
}

void UMyAttributeComponent::SetDefaultAttributes(
	float Health, float MaxHealth, float Stamina, float MaxStamina)
{
	if(!IsAbilitySystemValid() || !AttributeSet)
	{
		return;
	}

	// Use ApplyModToAttribute to set the base values
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetHealthAttribute(), EGameplayModOp::Override, Health);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
		MaxHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override, Stamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
		MaxStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetBaseDamageAttribute(), EGameplayModOp::Override,
		DefaultBaseDamage);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
		DefaultMaxWalkSpeed);
}