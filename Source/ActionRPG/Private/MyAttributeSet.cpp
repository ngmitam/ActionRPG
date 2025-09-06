// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeSet.h"
#include "GameplayEffectExtension.h"

UMyAttributeSet::UMyAttributeSet()
{
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
