// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
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
};
