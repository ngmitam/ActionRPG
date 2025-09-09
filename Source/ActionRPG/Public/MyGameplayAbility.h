// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "MyGameplayAbility.generated.h"

UCLASS()
class ACTIONRPG_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Ability Input ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	EMyAbilityInputID AbilityInputID = EMyAbilityInputID::None;

	// Buff Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	TArray<TSubclassOf<class UGameplayEffect>> BuffEffect;

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData);
	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

private:
	TArray<FActiveGameplayEffectHandle> BuffHandle;
};
