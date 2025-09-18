// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDamageEffect.h"
#include "MyAttributeSet.h"

UMyDamageEffect::UMyDamageEffect() {
  // Set up the effect to be instant
  DurationPolicy = EGameplayEffectDurationType::Instant;

  // Add modifier to subtract from Health
  FGameplayModifierInfo ModifierInfo;
  ModifierInfo.Attribute = UMyAttributeSet::GetHealthAttribute();
  ModifierInfo.ModifierOp = EGameplayModOp::Additive;
  FSetByCallerFloat SetByCaller;
  SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
  FGameplayEffectModifierMagnitude ModMag(SetByCaller);
  ModifierInfo.ModifierMagnitude = ModMag;
  Modifiers.Add(ModifierInfo);
}
