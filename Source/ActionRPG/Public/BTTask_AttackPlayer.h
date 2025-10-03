// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
#include "BTTask_AttackPlayer.generated.h"

class AMyEnemy;

/**
 * BT Task to attack the player if in range
 */
UCLASS()
class ACTIONRPG_API UBTTask_AttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AttackPlayer();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;

protected:
	// Blackboard key for the player
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerKey;

	// Blackboard key for attack range
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackRangeKey;
};
