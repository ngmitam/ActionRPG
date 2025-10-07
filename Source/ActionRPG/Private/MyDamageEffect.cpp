// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyDamageEffect.h"

#include "MyAbilityTypes.h"
#include "MyGameConfig.h"
#include "MyAttributeSet.h"

UMyDamageEffect::UMyDamageEffect()
{
	// Set up the effect to be instant
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Add modifier to subtract from Health using SetByCaller
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UMyAttributeSet::GetHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	// Use SetByCaller magnitude for dynamic damage (negative to subtract)
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
	// Set stun duration to configured value
	StunModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(
		FGameConfig::GetDefault().StunDuration);
	Modifiers.Add(StunModifierInfo);
}
