// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyAttributeComponent.h"

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectTypes.h"
#include "MyAbilitySystemComponent.h"
#include "MyBaseCharacter.h"
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
	// Don't initialize on default objects or templates
	if(!ValidateOwner())
	{
		return;
	}

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

	// Check for invalid object flags
	if(Owner->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		return false;
	}

	// Verify that the owner is properly initialized and not a template
	return Owner->IsActorInitialized() && !Owner->IsTemplate();
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
	if(RetryCount < FGameConfig::GetDefault().MaxInitializationRetries)
	{
		RetryCount++;
		ScheduleRetryInitialization();
	}
	else
	{
	}
}

void UMyAttributeComponent::InitializeAttributeComponent()
{
	// Initialize attribute component if it exists
	if(this)
	{
		// Defer initialization to ensure proper timing
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this, &UMyAttributeComponent::InitializeDefaultAttributes);
	}
}

void UMyAttributeComponent::InitializeDefaultAttributes()
{
	if(!IsAbilitySystemValid() || !AttributeSet)
	{
		return;
	}

	// Bind to health change delegate on the owner character
	if(AMyBaseCharacter *Character = Cast<AMyBaseCharacter>(GetOwner()))
	{
		GetOnHealthChanged().AddUObject(
			Character, &AMyBaseCharacter::OnHealthChanged);
	}

	// Always apply component properties to override attributes
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetHealthAttribute(), EGameplayModOp::Override,
		Health > 0.0f ? Health : FGameConfig::GetDefault().DefaultHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
		MaxHealth > 0.0f ? MaxHealth
						 : FGameConfig::GetDefault().DefaultMaxHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override,
		Stamina > 0.0f ? Stamina : FGameConfig::GetDefault().DefaultStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
		MaxStamina > 0.0f ? MaxStamina
						  : FGameConfig::GetDefault().DefaultMaxStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
		MaxWalkSpeed > 0.0f ? MaxWalkSpeed
							: FGameConfig::GetDefault().DefaultMaxWalkSpeed);
}

FDefaultAttributes UMyAttributeComponent::GetDefaultAttributes() const
{
	FDefaultAttributes DefaultAttrs;

	// Use the direct property values
	DefaultAttrs.Health = Health;
	DefaultAttrs.MaxHealth = MaxHealth;
	DefaultAttrs.Stamina = Stamina;
	DefaultAttrs.MaxStamina = MaxStamina;
	DefaultAttrs.MaxWalkSpeed = MaxWalkSpeed;

	return DefaultAttrs;
}

bool UMyAttributeComponent::CanInitializeAbilitySystem() const
{
	return IsAbilitySystemValid() && GetOwner() != nullptr;
}

void UMyAttributeComponent::InitializeAbilitySystem()
{
	if(!CanInitializeAbilitySystem())
	{
		return;
	}

	InitializeDefaultAttributes();
	InitializeAttributes();
	GiveDefaultAbilities();
	SetupAttributeChangeDelegates();
	SetInitialCharacterMovement();
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
	AActor *Owner = GetOwner();
	if(!Owner || !Owner->IsA<ACharacter>())
	{
		return;
	}

	if(ACharacter *Character = Cast<ACharacter>(Owner))
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

	// Attributes are now initialized in InitializeDefaultAttributes

	// Apply default attribute effects
	for(TSubclassOf<UGameplayEffect> GEClass : DefaultAttributeEffectClasses)
	{
		if(GEClass)
		{
			FGameplayEffectSpecHandle SpecHandle =
				AbilitySystemComponent->MakeOutgoingSpec(GEClass,
					FGameConfig::GetDefault().DefaultAbilityLevel,
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

float UMyAttributeComponent::GetStunDuration() const
{
	return AttributeSet ? AttributeSet->GetStunDuration() : 0.0f;
}

void UMyAttributeComponent::OnAttributeChange(
	const FOnAttributeChangeData &Data)
{
	AActor *Owner = GetOwner();
	if(!Owner)
	{
		return;
	}

	if(Data.Attribute == AttributeSet->GetMaxWalkSpeedAttribute())
	{
		if(Owner->IsA<ACharacter>())
		{
			if(ACharacter *Character = Cast<ACharacter>(Owner))
			{
				Character->GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
			}
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
		Owner->SetLifeSpan(FGameConfig::GetDefault().DeathLifeSpan);
	}
}

void UMyAttributeComponent::SetDefaultAttributes(
	const FDefaultAttributes &Attributes)
{
	if(!IsAbilitySystemValid() || !AttributeSet)
	{
		return;
	}

	// Always set attributes to the provided values
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetHealthAttribute(), EGameplayModOp::Override,
		Attributes.Health > 0.0f ? Attributes.Health
								 : FGameConfig::GetDefault().DefaultHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override,
		Attributes.MaxHealth > 0.0f
			? Attributes.MaxHealth
			: FGameConfig::GetDefault().DefaultMaxHealth);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetStaminaAttribute(), EGameplayModOp::Override,
		Attributes.Stamina > 0.0f ? Attributes.Stamina
								  : FGameConfig::GetDefault().DefaultStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxStaminaAttribute(), EGameplayModOp::Override,
		Attributes.MaxStamina > 0.0f
			? Attributes.MaxStamina
			: FGameConfig::GetDefault().DefaultMaxStamina);
	AbilitySystemComponent->ApplyModToAttribute(
		AttributeSet->GetMaxWalkSpeedAttribute(), EGameplayModOp::Override,
		Attributes.MaxWalkSpeed > 0.0f
			? Attributes.MaxWalkSpeed
			: FGameConfig::GetDefault().DefaultMaxWalkSpeed);
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