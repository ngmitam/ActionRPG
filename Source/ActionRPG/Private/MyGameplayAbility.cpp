// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameplayAbility.h"
#include "AbilitySystemComponent.h"

// Apply a buff effect when the ability is activated
void UMyGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (BuffEffect && ActorInfo->OwnerActor.IsValid())
    {
        FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(BuffEffect, GetAbilityLevel(Handle, ActorInfo));
        if (SpecHandle.IsValid())
        {
            BuffHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
        }
    }
}

// Remove the buff effect when the ability ends
void UMyGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (BuffHandle.IsValid() && ActorInfo->OwnerActor.IsValid())
    {
        UAbilitySystemComponent *AbilitySystem = ActorInfo->AbilitySystemComponent.Get();
        if (AbilitySystem)
        {
            AbilitySystem->RemoveActiveGameplayEffect(BuffHandle);
            BuffHandle.Invalidate();
        }
    }
}