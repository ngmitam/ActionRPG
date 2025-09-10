// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeComponent.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UMyAttributeComponent::UMyAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Attribute Set
    AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");
}

UAbilitySystemComponent *UMyAttributeComponent::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void UMyAttributeComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor *Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    // Initialize Ability System
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(Owner, Owner);

        InitializeAttributes();
        GiveDefaultAbilities();

        // Subscribe to attribute change callbacks
        if (AttributeSet)
        {
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxWalkSpeedAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UMyAttributeComponent::OnAttributeChange);

            // If owner is a character, set initial walk speed
            if (ACharacter *Character = Cast<ACharacter>(Owner))
            {
                Character->GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMaxWalkSpeed();
            }
        }
    }
}

void UMyAttributeComponent::InitializeAttributes()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    for (TSubclassOf<UGameplayEffect> DefaultAttributeEffect : DefaultAttributeEffectClasses)
    {
        if (!DefaultAttributeEffect)
        {
            continue;
        }
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.0f, FGameplayEffectContextHandle());
        if (SpecHandle.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
        }
    }
}

void UMyAttributeComponent::GiveDefaultAbilities()
{
    if (!AbilitySystemComponent || !GetOwner()->HasAuthority())
    {
        return;
    }
    for (TSubclassOf<UMyGameplayAbility> Ability : DefaultAbilityClasses)
    {
        AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), GetOwner()));
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
    }
}

void UMyAttributeComponent::OnStaminaChange(const FOnAttributeChangeData &Data)
{
    if (Data.NewValue <= 0.0f && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Sprinting"))))
    {
        // Out of stamina - stop sprinting
        SetSprinting(false);
        // Note: The actual ability cancellation should be handled by the ability system or character
    }
}
