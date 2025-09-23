// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"
#include "BTService_UpdateMovementAnimation.generated.h"

class AMyEnemy;

/**
 * BT Service to update movement animation based on velocity
 */
UCLASS()
class ACTIONRPG_API UBTService_UpdateMovementAnimation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateMovementAnimation();

	virtual void TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory,
		float DeltaSeconds) override;
};
