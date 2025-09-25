// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDamageEffect.h"

#include "MyAttributeSet.h"

UMyDamageEffect::UMyDamageEffect()
{
	// Set up the effect to be instant
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Add modifier to subtract from Health using SetByCaller
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UMyAttributeSet::GetHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	// Use SetByCaller magnitude for dynamic damage
	FSetByCallerFloat SetByCallerFloat;
	SetByCallerFloat.DataTag =
		FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	ModifierInfo.ModifierMagnitude =
		FGameplayEffectModifierMagnitude(SetByCallerFloat);
	Modifiers.Add(ModifierInfo);

	// Add modifier to set StunDuration
	FGameplayModifierInfo StunModifierInfo;
	StunModifierInfo.Attribute = UMyAttributeSet::GetStunDurationAttribute();
	StunModifierInfo.ModifierOp = EGameplayModOp::Override;
	// Set stun duration to 2 seconds
	StunModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(2.0f);
	Modifiers.Add(StunModifierInfo);
}
