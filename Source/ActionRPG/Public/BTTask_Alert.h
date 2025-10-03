// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Alert.generated.h"

/**
 *
 */
UCLASS()
class ACTIONRPG_API UBTTask_Alert : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Alert();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;
};
