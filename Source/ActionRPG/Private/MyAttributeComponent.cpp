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
	if(!IsAbilitySystemComponentRegistered())
	{
		// Component not created or registered, try again next frame
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &UMyAttributeComponent::DeferredInitialize);
		return;
	}

	// Try to initialize AbilityActorInfo if not already done
	if(!TryInitializeAbilityActorInfo())
	{
		HandleInitializationRetry();
		return;
	}

	// Initialize GAS components
	InitializeGASComponents();

	// Double-check that AbilityActorInfo is now valid
	if(IsAbilityActorInfoValid())
	{
		InitializeAbilitySystem();
	}
	else
	{
		HandleInitializationRetry();
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

bool UMyAttributeComponent::IsAbilitySystemComponentRegistered() const
{
	return AbilitySystemComponent && AbilitySystemComponent->IsRegistered();
}

bool UMyAttributeComponent::IsAbilityActorInfoValid() const
{
	return AbilitySystemComponent
		   && AbilitySystemComponent->AbilityActorInfo.IsValid();
}

bool UMyAttributeComponent::TryInitializeAbilityActorInfo()
{
	if(!IsAbilityActorInfoValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
		return IsAbilityActorInfoValid();
	}
	return true;
}

void UMyAttributeComponent::HandleInitializationRetry()
{
	// Still not ready, try again next frame (with a limit to prevent
	// infinite loops)
	static int32 RetryCount = 0;
	if(RetryCount < DefaultValues::MaxInitializationRetries)
	{
		RetryCount++;
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &UMyAttributeComponent::DeferredInitialize);
	}
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
	FDefaultAttributes DefaultAttrs;
	DefaultAttrs.Health = DefaultHealth;
	DefaultAttrs.MaxHealth = DefaultMaxHealth;
	DefaultAttrs.Stamina = DefaultStamina;
	DefaultAttrs.MaxStamina = DefaultMaxStamina;
	DefaultAttrs.BaseDamage = DefaultBaseDamage;
	DefaultAttrs.MaxWalkSpeed = DefaultMaxWalkSpeed;

	SetDefaultAttributes(DefaultAttrs);
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
		FGameplayAbilitySpecHandle Handle =
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

float UMyAttributeComponent::GetStunDuration() const
{
	return AttributeSet ? AttributeSet->GetStunDuration() : 0.0f;
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
			FGameplayTag::RequestGameplayTag(StateTags::Sprinting)))
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
		Owner->SetLifeSpan(
			DefaultValues::DeathLifeSpan); // Destroy after 2 seconds
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

void UMyAttributeComponent::SetDefaultAttributes(
	const FDefaultAttributes &Attributes)
{
	if(!IsAbilitySystemValid() || !AttributeSet)
	{
		return;
	}

	// Use ApplyModToAttribute to set the base values
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetHealthAttribute(), EGameplayModOp::Override,
		Attributes.Health);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
		Attributes.MaxHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override,
		Attributes.Stamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
		Attributes.MaxStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetBaseDamageAttribute(), EGameplayModOp::Override,
		Attributes.BaseDamage);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
		Attributes.MaxWalkSpeed);
}