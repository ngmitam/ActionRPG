// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayEffect.h"
#include "MyGameConfig.h"
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
	float TraceLength = FGameConfig::GetDefault().AttackTraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float TraceRadius = FGameConfig::GetDefault().AttackTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector TraceOffset = FVector(FGameConfig::GetDefault().AttackTraceOffsetX,
		FGameConfig::GetDefault().AttackTraceOffsetY,
		FGameConfig::GetDefault().AttackTraceOffsetZ);

	// Damage properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

protected:
	void PerformTraceAndApplyDamage(AActor *OwnerActor);
};
