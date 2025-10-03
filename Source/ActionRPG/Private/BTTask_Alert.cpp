// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_Alert.h"

#include "AIController.h"
#include "MyEnemyAIController.h"

UBTTask_Alert::UBTTask_Alert()
{
	NodeName = "Alert";
}

EBTNodeResult::Type UBTTask_Alert::ExecuteTask(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyEnemyAIController *EnemyController =
		Cast<AMyEnemyAIController>(AIController);
	if(!EnemyController)
	{
		return EBTNodeResult::Failed;
	}

	EnemyController->AlertNearbyEnemies();

	return EBTNodeResult::Succeeded;
}
