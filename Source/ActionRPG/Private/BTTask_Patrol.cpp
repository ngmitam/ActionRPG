// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_Patrol.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
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

	// Check for player detection
	AMyEnemyAIController *EnemyController =
		Cast<AMyEnemyAIController>(AIController);
	if(!EnemyController)
	{
		return;
	}

	// Find player character
	ACharacter *PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);
	if(!PlayerCharacter)
	{
		return;
	}

	// Check distance for detection
	float PlayerDistance =
		FVector::Dist(AIController->GetPawn()->GetActorLocation(),
			PlayerCharacter->GetActorLocation());
	if(PlayerDistance > EnemyController->DetectionRange)
	{
		return;
	}

	// Check line of sight
	UWorld *World = AIController->GetWorld();
	if(!World)
	{
		return;
	}

	FVector Start = AIController->GetPawn()->GetActorLocation();
	FVector End = PlayerCharacter->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AIController->GetPawn()); // Ignore self

	bool bHit = World->LineTraceSingleByChannel(
		HitResult, Start, End, ECC_Visibility, QueryParams);

	if(bHit && HitResult.GetActor() != PlayerCharacter)
	{
		// Something is blocking the line of sight
		return;
	}

	// Player detected, set in blackboard and finish patrol
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(
			PlayerKey.SelectedKeyName, PlayerCharacter);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
