// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_Patrol.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyEnemyAIController.h"
#include "MyEnemy.h"
#include "NavigationSystem.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = "Patrol";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy || Enemy->PatrolPoints.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	// Move to next patrol point
	FVector TargetLocation =
		Enemy->PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
	AIController->MoveToLocation(TargetLocation);

	return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check if reached current patrol point
	FVector CurrentLocation = AIController->GetPawn()->GetActorLocation();
	FVector TargetLocation =
		Enemy->PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	if(Distance
		< FGameConfig::GetDefault().PatrolPointTolerance) // Close enough
	{
		// Move to next point
		CurrentPatrolIndex =
			(CurrentPatrolIndex + 1) % Enemy->PatrolPoints.Num();
		TargetLocation =
			Enemy->PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
		AIController->MoveToLocation(TargetLocation);
	}

	// Check if alerted
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(BlackboardComp && BlackboardComp->GetValueAsBool(FName("Alerted")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
