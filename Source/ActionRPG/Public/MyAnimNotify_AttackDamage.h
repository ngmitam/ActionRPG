// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayEffect.h"
#include "MyAnimNotify_AttackDamage.generated.h"

/**
 * Animation notify to perform attack trace and apply damage
 */
UCLASS(Blueprintable)
class ACTIONRPG_API UMyAnimNotify_AttackDamage : public UAnimNotify
{
	GENERATED_BODY()

public:
	UMyAnimNotify_AttackDamage();

	virtual void Notify(USkeletalMeshComponent *MeshComp,
		UAnimSequenceBase *Animation,
		const FAnimNotifyEventReference &EventReference) override;

	// Trace properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float TraceLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float TraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector TraceOffset =
		FVector(100.0f, 0.0f, 0.0f); // Offset from character location

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

protected:
	void PerformTraceAndApplyDamage(AActor *OwnerActor);
};
