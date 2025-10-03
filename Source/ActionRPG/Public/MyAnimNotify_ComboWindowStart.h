// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
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

	virtual void Notify(USkeletalMeshComponent *MeshComp,
		UAnimSequenceBase *Animation) override;
};
