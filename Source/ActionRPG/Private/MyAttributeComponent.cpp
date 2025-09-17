// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeComponent.h"
#include "CoreMinimal.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitiesModule.h"
#include "AbilitySystemInterface.h"

UMyAttributeComponent::UMyAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Ability System Component - moved to constructor for proper initialization timing
    AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
    // Removed SetIsReplicated and SetReplicationMode from constructor to avoid timing issues
    // They will be set in DeferredInitialize after AbilityActorInfo is initialized

    // Attribute Set will be created in DeferredInitialize with proper outer
}

UAbilitySystemComponent *UMyAttributeComponent::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void UMyAttributeComponent::OnRegister()
{
    Super::OnRegister();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RegisterComponent();
    }

    // Component registration - no GAS initialization here to avoid timing issues
}

void UMyAttributeComponent::BeginPlay()
{
    Super::BeginPlay();

    // Always defer GAS initialization to ensure proper timing
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMyAttributeComponent::DeferredInitialize);
}

void UMyAttributeComponent::DeferredInitialize()
{
    // Verify that we have a valid owner (should be the character)
    AActor *Owner = GetOwner();
    if (!Owner)
    {
        // If no owner, try again next frame
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMyAttributeComponent::DeferredInitialize);
        return;
    }

    // Verify that the owner is properly initialized
    if (!Owner->IsActorInitialized())
    {
        // Owner not fully initialized, try again next frame
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMyAttributeComponent::DeferredInitialize);
        return;
    }

    // Ensure AbilitySystemComponent exists and is registered
    if (!AbilitySystemComponent || !AbilitySystemComponent->IsRegistered())
    {
        // Component not created or registered, try again next frame
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMyAttributeComponent::DeferredInitialize);
        return;
    }

    // Initialize AbilityActorInfo if not already done
    if (!AbilitySystemComponent->AbilityActorInfo.IsValid())
    {
        AbilitySystemComponent->InitAbilityActorInfo(Owner, Owner);
    }

    // Create AttributeSet with Owner as outer
    if (!AttributeSet)
    {
        AttributeSet = NewObject<UMyAttributeSet>(Owner);
    }

    // Add AttributeSet to AbilitySystemComponent if not already added
    if (AttributeSet && !AbilitySystemComponent->HasAttributeSetForAttribute(AttributeSet->GetHealthAttribute()))
    {
        AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);
    }

    // Set replication properties after AbilityActorInfo is initialized
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Double-check that AbilityActorInfo is now valid
    if (AbilitySystemComponent->AbilityActorInfo.IsValid())
    {
        InitializeAbilitySystem();
    }
    else
    {
        // Still not ready, try again next frame (with a limit to prevent infinite loops)
        static int RetryCount = 0;
        if (RetryCount < 10) // Limit retries to prevent infinite loop
        {
            RetryCount++;
            GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMyAttributeComponent::DeferredInitialize);
        }
    }
}

void UMyAttributeComponent::InitializeAbilitySystem()
{
    // Final validation before initializing
    if (!AbilitySystemComponent || !AbilitySystemComponent->AbilityActorInfo.IsValid() || !GetOwner())
    {
        return;
    }

    // At this point, we know AbilitySystemComponent and AbilityActorInfo are valid
    // and the owner is initialized, so we can safely proceed

    // Now it's safe to initialize attributes and abilities
    InitializeAttributes();
    GiveDefaultAbilities();

    // Subscribe to attribute change callbacks
    if (AttributeSet)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxWalkSpeedAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);

        // If owner is a character, set initial walk speed
        if (ACharacter *Character = Cast<ACharacter>(GetOwner()))
        {
            Character->GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMaxWalkSpeed();
        }
    }
}

void UMyAttributeComponent::InitializeAttributes()
{
    if (!AbilitySystemComponent || !AbilitySystemComponent->AbilityActorInfo.IsValid())
    {
        return;
    }

    for (TSubclassOf<UGameplayEffect> DefaultAttributeEffect : DefaultAttributeEffectClasses)
    {
        if (!DefaultAttributeEffect)
        {
            continue;
        }

        // Validate the effect class
        if (!DefaultAttributeEffect.GetDefaultObject())
        {
            continue;
        }

        // Create proper context with Actor as instigator
        FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
        AActor *Owner = Cast<AActor>(GetOwner());
        if (Owner)
        {
            FGameplayEffectContext *Context = ContextHandle.Get();
            if (Context)
            {
                Context->AddInstigator(Owner, Owner);
                TArray<TWeakObjectPtr<AActor>> Targets;
                Targets.Add(Owner);
                Context->AddActors(Targets);
            }
        }
        else
        {
            // Fallback: use the component itself as instigator if owner is not an Actor
            // Note: AddInstigator requires AActor, so we can't use the component directly
        }

        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.0f, ContextHandle);
        if (SpecHandle.IsValid())
        {
            // Apply to self using the self-application method
            FGameplayEffectSpec *Spec = SpecHandle.Data.Get();
            if (Spec)
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
            }
        }
    }
}

void UMyAttributeComponent::GiveDefaultAbilities()
{
    if (!AbilitySystemComponent || !AbilitySystemComponent->AbilityActorInfo.IsValid() || !GetOwner()->HasAuthority())
    {
        return;
    }

    for (int32 i = 0; i < DefaultAbilityClasses.Num(); ++i)
    {
        TSubclassOf<UMyGameplayAbility> Ability = DefaultAbilityClasses[i];

        if (!Ability)
        {
            continue;
        }

        // Validate the ability class
        UMyGameplayAbility *AbilityCDO = Ability.GetDefaultObject();
        if (!AbilityCDO)
        {
            continue;
        }

        // Validate AbilityInputID
        int32 InputID = static_cast<int32>(AbilityCDO->AbilityInputID);
        if (InputID < 0)
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

void UMyAttributeComponent::OnAttributeChange(const FOnAttributeChangeData &Data)
{
    if (Data.Attribute == AttributeSet->GetMaxWalkSpeedAttribute())
    {
        if (ACharacter *Character = Cast<ACharacter>(GetOwner()))
        {
            Character->GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
        }
    }
    else if (Data.Attribute == AttributeSet->GetStaminaAttribute())
    {
        OnStaminaChange(Data);
    }
    else if (Data.Attribute == AttributeSet->GetHealthAttribute())
    {
        // Handle health changes if needed, e.g., update UI or trigger events
        if (Data.NewValue <= 0.0f)
        {
            HandleDeath();
        }
    }
}

void UMyAttributeComponent::OnStaminaChange(const FOnAttributeChangeData &Data)
{
    if (Data.NewValue <= 0.0f && AbilitySystemComponent &&
        AbilitySystemComponent->AbilityActorInfo.IsValid() &&
        AbilitySystemComponent->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(FName("State.Sprinting"))))
    {
        // Out of stamina - stop sprinting
        SetSprinting(false);
        // Note: The actual ability cancellation should be handled by the ability system or character
    }
}

void UMyAttributeComponent::HandleDeath()
{
    AActor *Owner = GetOwner();
    if (Owner)
    {
        // Default: destroy actor after a delay
        Owner->SetLifeSpan(2.0f); // Destroy after 2 seconds
    }
}
