// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDamageEffect.h"

#include "MyAttributeSet.h"

UMyDamageEffect::UMyDamageEffect()
{
	// Set up the effect to be instant
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Add modifier to subtract from Health
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UMyAttributeSet::GetHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	// Use a default magnitude, will be overridden in code
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(-10.0f);
	Modifiers.Add(ModifierInfo);
}
