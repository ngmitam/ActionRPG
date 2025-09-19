// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MyDamageEffect.generated.h"

/**
 * Damage effect for attack abilities
 */
UCLASS()
class ACTIONRPG_API UMyDamageEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UMyDamageEffect();

	// Damage magnitude (set in editor or via blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 10.0f;

	// Damage type tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayTag DamageTypeTag;
};
