// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

UMyAttackAbility::UMyAttackAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UMyAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    PlayAttackMontage();
    ApplyDamageEffect();
}

void UMyAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMyAttackAbility::OnAttackInputPressed()
{
    if (CanCombo())
    {
        AdvanceCombo();
    }
}

bool UMyAttackAbility::CanCombo() const
{
    return bInComboWindow && CurrentComboIndex < GetMaxComboIndex();
}

void UMyAttackAbility::AdvanceCombo()
{
    if (CanCombo())
    {
        CurrentComboIndex++;
        bInComboWindow = false;

        // Queue the next combo montage to play after current montage ends
        bComboQueued = true;

        // Apply damage effect immediately
        ApplyDamageEffect();
    }
}

void UMyAttackAbility::StartComboWindow()
{
    if (CurrentComboIndex < GetMaxComboIndex())
    {
        bInComboWindow = true;
    }
}

void UMyAttackAbility::EndComboWindow()
{
    bInComboWindow = false;
}

void UMyAttackAbility::ResetCombo()
{
    CurrentComboIndex = 0;
    bInComboWindow = false;
    bComboQueued = false;
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UMyAttackAbility::PlayAttackMontage()
{
    if (!AttackMontages.IsValidIndex(CurrentComboIndex) || !AttackMontages[CurrentComboIndex] ||
        !GetCurrentActorInfo()->OwnerActor.IsValid())
    {
        return;
    }

    ACharacter *Character = Cast<ACharacter>(GetCurrentActorInfo()->OwnerActor.Get());
    if (!Character)
    {
        return;
    }

    USkeletalMeshComponent *Mesh = Character->GetMesh();
    if (!Mesh)
    {
        return;
    }

    UAnimInstance *AnimInstance = Mesh->GetAnimInstance();
    if (!AnimInstance)
    {
        return;
    }

    UAnimMontage *Montage = AttackMontages[CurrentComboIndex];

    AnimInstance->Montage_Stop(0.0f);
    float Duration = AnimInstance->Montage_Play(Montage);

    if (Duration > 0.f)
    {
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UMyAttackAbility::OnMontageEnded);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
    }
}

void UMyAttackAbility::ApplyDamageEffect()
{
    if (DamageEffects.IsValidIndex(CurrentComboIndex))
    {
        TSubclassOf<UGameplayEffect> DamageEffect = DamageEffects[CurrentComboIndex];
        if (DamageEffect)
        {
            FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
            const FGameplayAbilityActorInfo *ActorInfo = GetCurrentActorInfo();
            const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

            ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, DamageEffect.GetDefaultObject(), GetAbilityLevel(Handle, ActorInfo));
        }
    }
}

void UMyAttackAbility::OnMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (bComboQueued)
        {
            PlayAttackMontage();
            bComboQueued = false;
        }
        else if (!bInComboWindow)
        {
            ResetCombo();
        }
    }
}
