// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyBossAttackNotify.generated.h"

UCLASS()
class ACTIONRPG_API UMyBossAttackNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent *MeshComp,
		UAnimSequenceBase *Animation) override;
};
