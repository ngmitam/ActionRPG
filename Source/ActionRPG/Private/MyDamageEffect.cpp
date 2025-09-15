// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDamageEffect.h"

UMyDamageEffect::UMyDamageEffect()
{
    // Set up the effect to be instant
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(0.0f); // Instant
}
