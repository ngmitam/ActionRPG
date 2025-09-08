// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UMyAttributeSet::UMyAttributeSet()
{
    // Initialize default values for attributes
    Health = 100.0f;
    MaxHealth = 100.0f;
    Stamina = 100.0f;
    MaxStamina = 100.0f;
    BaseDamage = 10.0f;
    MaxWalkSpeed = 300.0f; // Default walk speed
}

void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxWalkSpeed, COND_None, REPNOTIFY_Always);
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Check if the attribute being modified is Health.
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // Clamp Health between 0 and MaxHealth.
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }

    // Check if the attribute being modified is Stamina.
    if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        // Clamp Stamina between 0 and MaxStamina.
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
}

void UMyAttributeSet::OnRep_MaxWalkSpeed(const FGameplayAttributeData &OldMaxWalkSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxWalkSpeed, OldMaxWalkSpeed);
}
