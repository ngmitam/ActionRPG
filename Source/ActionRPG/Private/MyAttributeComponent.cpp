// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyAttributeComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
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
	if(!IsReadyForInitialization())
	{
		ScheduleRetryInitialization();
		return;
	}

	if(!TryInitializeAbilityActorInfo())
	{
		HandleInitializationRetry();
		return;
	}

	InitializeGASComponents();

	if(IsAbilityActorInfoValid())
	{
		InitializeAbilitySystem();
	}
	else
	{
		HandleInitializationRetry();
	}
}

bool UMyAttributeComponent::IsReadyForInitialization() const
{
	return ValidateOwner() && IsAbilitySystemComponentRegistered();
}

void UMyAttributeComponent::ScheduleRetryInitialization()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		this, &UMyAttributeComponent::DeferredInitialize);
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
	static int32 RetryCount = 0;
	if(RetryCount < DefaultValues::MaxInitializationRetries)
	{
		RetryCount++;
		ScheduleRetryInitialization();
	}
	else
	{
		// Log error after max retries
		UE_LOG(LogTemp, Error,
			TEXT("Failed to initialize Ability System Component after %d "
				 "retries"),
			DefaultValues::MaxInitializationRetries);
	}
}

void UMyAttributeComponent::InitializeAbilitySystem()
{
	if(!CanInitializeAbilitySystem())
	{
		return;
	}

	InitializeAttributes();
	GiveDefaultAbilities();
	SetupAttributeChangeDelegates();
	SetInitialCharacterMovement();
}

bool UMyAttributeComponent::CanInitializeAbilitySystem() const
{
	return IsAbilitySystemValid() && GetOwner() != nullptr;
}

void UMyAttributeComponent::SetupAttributeChangeDelegates()
{
	if(!AttributeSet)
	{
		return;
	}

	// Subscribe to attribute change callbacks
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
}

void UMyAttributeComponent::SetInitialCharacterMovement()
{
	if(ACharacter *Character = Cast<ACharacter>(GetOwner()))
	{
		Character->GetCharacterMovement()->MaxWalkSpeed =
			AttributeSet->GetMaxWalkSpeed();
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
	DefaultAttrs.MaxWalkSpeed = DefaultMaxWalkSpeed;

	SetDefaultAttributes(DefaultAttrs);

	// Apply default attribute effects
	for(TSubclassOf<UGameplayEffect> GEClass : DefaultAttributeEffectClasses)
	{
		if(GEClass)
		{
			FGameplayEffectSpecHandle SpecHandle =
				AbilitySystemComponent->MakeOutgoingSpec(GEClass,
					GameplayConstants::DefaultAbilityLevel,
					FGameplayEffectContextHandle());
			if(SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					*SpecHandle.Data.Get(), AbilitySystemComponent);
			}
		}
	}
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

	// Only set attributes if they haven't been set before
	if(!IsAttributeInitialized(AttributeSet->GetHealthAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetHealthAttribute(), EGameplayModOp::Override,
			Health);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxHealthAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
			MaxHealth);
	}
	if(!IsAttributeInitialized(AttributeSet->GetStaminaAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override,
			Stamina);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxStaminaAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
			MaxStamina);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxWalkSpeedAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
			DefaultMaxWalkSpeed);
	}
}

void UMyAttributeComponent::SetDefaultAttributes(
	const FDefaultAttributes &Attributes)
{
	if(!IsAbilitySystemValid() || !AttributeSet)
	{
		return;
	}

	// Only set attributes if they haven't been set before
	if(!IsAttributeInitialized(AttributeSet->GetHealthAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetHealthAttribute(), EGameplayModOp::Override,
			Attributes.Health);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxHealthAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
			Attributes.MaxHealth);
	}
	if(!IsAttributeInitialized(AttributeSet->GetStaminaAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override,
			Attributes.Stamina);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxStaminaAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
			Attributes.MaxStamina);
	}
	if(!IsAttributeInitialized(AttributeSet->GetMaxWalkSpeedAttribute()))
	{
		AbilitySystemComponent->ApplyModToAttribute(
			AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
			Attributes.MaxWalkSpeed);
	}
}

bool UMyAttributeComponent::IsAttributeInitialized(
	const FGameplayAttribute &Attribute) const
{
	if(!HasValidAttributeSet())
	{
		return false;
	}

	// Check if the attribute has been set (not still 0.0f)
	if(Attribute == AttributeSet->GetHealthAttribute())
	{
		return AttributeSet->GetHealth() > 0.0f;
	}
	if(Attribute == AttributeSet->GetMaxHealthAttribute())
	{
		return AttributeSet->GetMaxHealth() > 0.0f;
	}
	if(Attribute == AttributeSet->GetStaminaAttribute())
	{
		return AttributeSet->GetStamina() > 0.0f;
	}
	if(Attribute == AttributeSet->GetMaxStaminaAttribute())
	{
		return AttributeSet->GetMaxStamina() > 0.0f;
	}
	if(Attribute == AttributeSet->GetMaxWalkSpeedAttribute())
	{
		return AttributeSet->GetMaxWalkSpeed() > 0.0f;
	}

	return true; // For other attributes, assume initialized
}