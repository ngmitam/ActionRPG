// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttackAbility.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "MyDamageEffect.h"
#include "MyEnemy.h"
#include "WorldCollision.h"

UMyAttackAbility::UMyAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UMyAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayAttackMontage();
	// Damage is now applied via animation notify
}

void UMyAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(
		Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMyAttackAbility::OnAttackInputPressed()
{
	if(CanCombo())
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
	if(CanCombo())
	{
		CurrentComboIndex++;
		bInComboWindow = false;

		// Queue the next combo montage to play after current montage ends
		bComboQueued = true;

		// Damage is now applied via animation notify
	}
}

void UMyAttackAbility::StartComboWindow()
{
	if(CurrentComboIndex < GetMaxComboIndex())
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
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), true, false);
}

void UMyAttackAbility::PlayAttackMontage()
{
	if(!AttackMontages.IsValidIndex(CurrentComboIndex)
		|| !AttackMontages[CurrentComboIndex]
		|| !GetCurrentActorInfo()->OwnerActor.IsValid())
	{
		return;
	}

	ACharacter *Character =
		Cast<ACharacter>(GetCurrentActorInfo()->OwnerActor.Get());
	if(!Character)
	{
		return;
	}

	USkeletalMeshComponent *Mesh = Character->GetMesh();
	if(!Mesh)
	{
		return;
	}

	UAnimInstance *AnimInstance = Mesh->GetAnimInstance();
	if(!AnimInstance)
	{
		return;
	}

	UAnimMontage *Montage = AttackMontages[CurrentComboIndex];

	AnimInstance->Montage_Stop(0.0f);
	float Duration = AnimInstance->Montage_Play(Montage);

	if(Duration > 0.f)
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(
			this, &UMyAttackAbility::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	}
}

void UMyAttackAbility::ApplyDamageEffect()
{
	TSubclassOf<UGameplayEffect> DamageEffectToUse = nullptr;

	if(DamageEffects.IsValidIndex(CurrentComboIndex))
	{
		DamageEffectToUse = DamageEffects[CurrentComboIndex];
	}

	// If no damage effect set, use default
	if(!DamageEffectToUse)
	{
		DamageEffectToUse = UMyDamageEffect::StaticClass();
	}

	if(DamageEffectToUse)
	{
		FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
		const FGameplayAbilityActorInfo *ActorInfo = GetCurrentActorInfo();
		const FGameplayAbilityActivationInfo ActivationInfo =
			GetCurrentActivationInfo();

		// Find target (enemy in range)
		AActor *Target = FindTargetInRange();
		if(Target)
		{
			// Apply to target
			FGameplayEffectSpecHandle DamageSpecHandle =
				GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
					DamageEffectToUse, GetAbilityLevel(Handle, ActorInfo),
					FGameplayEffectContextHandle());
			if(DamageSpecHandle.IsValid())
			{
				// Create target data
				FGameplayAbilityTargetData_SingleTargetHit *TargetData =
					new FGameplayAbilityTargetData_SingleTargetHit();
				TargetData->HitResult = FHitResult(
					Target, nullptr, FVector::ZeroVector, FVector::ZeroVector);
				FGameplayAbilityTargetDataHandle TargetDataHandle(TargetData);

				// Apply to target
				ApplyGameplayEffectSpecToTarget(Handle, ActorInfo,
					ActivationInfo, DamageSpecHandle, TargetDataHandle);
			}
		}
		else
		{
			// Apply to owner if no target (for now)
			ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo,
				DamageEffectToUse.GetDefaultObject(),
				GetAbilityLevel(Handle, ActorInfo));
		}
	}
}

AActor *UMyAttackAbility::FindTargetInRange()
{
	ACharacter *Character =
		Cast<ACharacter>(GetCurrentActorInfo()->OwnerActor.Get());
	if(!Character)
	{
		return nullptr;
	}

	FVector Location = Character->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRange);
	GetWorld()->OverlapMultiByChannel(
		OverlapResults, Location, FQuat::Identity, ECC_Pawn, Sphere);

	for(const FOverlapResult &Result : OverlapResults)
	{
		AActor *Actor = Result.GetActor();
		if(Actor && Actor != Character && Actor->IsA(AMyEnemy::StaticClass()))
		{
			return Actor;
		}
	}

	return nullptr;
}

void UMyAttackAbility::OnMontageEnded(UAnimMontage *Montage, bool bInterrupted)
{
	if(!bInterrupted)
	{
		if(bComboQueued)
		{
			PlayAttackMontage();
			bComboQueued = false;
		}
		else if(!bInComboWindow)
		{
			ResetCombo();
		}
	}
}
