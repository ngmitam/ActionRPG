// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "MyAbilityTypes.h"
#include "MyGameplayAbility.generated.h"

UCLASS()
class ACTIONRPG_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Ability Input ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	EMyAbilityInputID AbilityInputID = EMyAbilityInputID::None;

	// Buff effect classes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buff")
	TArray<TSubclassOf<class UGameplayEffect>> BuffEffectClasses;

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData *TriggerEventData);
	void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled);

private:
	TArray<FActiveGameplayEffectHandle> BuffHandle;
};
