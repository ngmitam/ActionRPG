// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyAnimNotify_ComboWindowStart.generated.h"

/**
 * Animation notify to start combo window
 */
UCLASS()
class ACTIONRPG_API UMyAnimNotify_ComboWindowStart : public UAnimNotify
{
    GENERATED_BODY()

public:
    UMyAnimNotify_ComboWindowStart();

    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation) override;
};
