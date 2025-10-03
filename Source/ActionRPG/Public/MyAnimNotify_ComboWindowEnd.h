// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "MyAnimNotify_ComboWindowEnd.generated.h"

/**
 * Animation notify to end combo window
 */
UCLASS()
class ACTIONRPG_API UMyAnimNotify_ComboWindowEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	UMyAnimNotify_ComboWindowEnd();

	virtual void Notify(USkeletalMeshComponent *MeshComp,
		UAnimSequenceBase *Animation) override;
};
