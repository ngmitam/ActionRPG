// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

// Apply a buff effect when the ability is activated
void UMyGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData) {
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  // Commit the ability
  CommitAbility(Handle, ActorInfo, ActivationInfo);

  if (BuffEffectClasses.Num() > 0 && ActorInfo->OwnerActor.IsValid()) {
    for (const TSubclassOf<UGameplayEffect> &Effect : BuffEffectClasses) {
      if (Effect) {
        FActiveGameplayEffectHandle BuffHandleTemp = ApplyGameplayEffectToOwner(
            Handle, ActorInfo, ActivationInfo, Effect.GetDefaultObject(),
            GetAbilityLevel(Handle, ActorInfo));
        BuffHandle.Add(BuffHandleTemp);
      }
    }
  }
}

// Remove the buff effect when the ability ends
void UMyGameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled) {
  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
                    bWasCancelled);

  for (FActiveGameplayEffectHandle &ActiveHandle : BuffHandle) {
    if (ActiveHandle.IsValid() && ActorInfo->OwnerActor.IsValid()) {
      UAbilitySystemComponent *AbilitySystem =
          ActorInfo->AbilitySystemComponent.Get();
      if (AbilitySystem) {
        AbilitySystem->RemoveActiveGameplayEffect(ActiveHandle);
        ActiveHandle.Invalidate();
      }
    }
  }
  BuffHandle.Empty();
}