// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSet.h"
#include "MyGameplayAbility.h"
#include "GameFramework/PlayerState.h"

AMyCharacter::AMyCharacter()
{
    // Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Attribute Set
    AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");
}

UAbilitySystemComponent *AMyCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AMyCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    // --- Server-side Initialization ---
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        InitializeAttributes();
        GiveDefaultAbilities();
    }
}

void AMyCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // --- Client-side Initialization ---
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMyCharacter::InitializeAttributes()
{
    if (!AbilitySystemComponent || !DefaultAttributeEffect)
    {
        return;
    }

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

    if (SpecHandle.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void AMyCharacter::GiveDefaultAbilities()
{
    if (!AbilitySystemComponent || !HasAuthority())
    {
        return;
    }
}