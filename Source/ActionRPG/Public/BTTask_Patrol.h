// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Patrol.generated.h"

/**
 *
 */
UCLASS()
class ACTIONRPG_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory,
		float DeltaSeconds) override;

private:
	int32 CurrentPatrolIndex = 0;
};
