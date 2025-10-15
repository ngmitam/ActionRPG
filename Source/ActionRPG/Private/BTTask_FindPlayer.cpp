// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_FindPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "MyEnemy.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_FindPlayer::UBTTask_FindPlayer()
{
	NodeName = "Find Player";
	bNotifyTick = true;
	bHasTarget = false;
}

EBTNodeResult::Type UBTTask_FindPlayer::ExecuteTask(
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

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy || !Enemy->IsActivated())
	{
		return EBTNodeResult::Failed;
	}

	// Find player character
	ACharacter *PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);
	if(!PlayerCharacter)
	{
		return EBTNodeResult::Failed;
	}

	// Check distance for detection
	float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(),
		PlayerCharacter->GetActorLocation());
	if(Distance > EnemyController->DetectionRange)
	{
		// Player not in range, start movement
		StartMovement(AIController, Enemy);
		return EBTNodeResult::InProgress;
	}

	// Check line of sight
	UWorld *World = AIController->GetWorld();
	if(!World)
	{
		return EBTNodeResult::Failed;
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
		// Something is blocking the line of sight, continue movement
		StartMovement(AIController, Enemy);
		return EBTNodeResult::InProgress;
	}

	// Set player in blackboard
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(
			PlayerKey.SelectedKeyName, PlayerCharacter);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_FindPlayer::TickTask(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AMyEnemyAIController *EnemyController =
		Cast<AMyEnemyAIController>(AIController);
	if(!EnemyController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy || !Enemy->IsActivated())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Find player character
	ACharacter *PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);
	if(!PlayerCharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check distance for detection
	float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(),
		PlayerCharacter->GetActorLocation());
	if(Distance <= EnemyController->DetectionRange)
	{
		// Check line of sight
		UWorld *World = AIController->GetWorld();
		if(World)
		{
			FVector Start = AIController->GetPawn()->GetActorLocation();
			FVector End = PlayerCharacter->GetActorLocation();

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(AIController->GetPawn()); // Ignore self

			bool bHit = World->LineTraceSingleByChannel(
				HitResult, Start, End, ECC_Visibility, QueryParams);

			if(!bHit || HitResult.GetActor() == PlayerCharacter)
			{
				// Player detected, set in blackboard
				UBlackboardComponent *BlackboardComp =
					OwnerComp.GetBlackboardComponent();
				if(BlackboardComp)
				{
					BlackboardComp->SetValueAsObject(
						PlayerKey.SelectedKeyName, PlayerCharacter);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return;
				}
			}
		}
	}

	// Continue random movement
	if(bHasTarget)
	{
		EPathFollowingStatus::Type Status = AIController->GetMoveStatus();
		if(Status == EPathFollowingStatus::Idle)
		{
			bHasTarget = false;
			StartMovement(AIController, Enemy);
		}
	}
	else
	{
		StartMovement(AIController, Enemy);
	}
}

void UBTTask_FindPlayer::StartMovement(
	AAIController *AIController, AMyEnemy *Enemy)
{
	if(!AIController || !Enemy)
	{
		return;
	}

	if(Enemy->PatrolPoints.Num() > 0)
	{
		// Patrol mode: move to next patrol point
		if(CurrentPatrolIndex >= Enemy->PatrolPoints.Num())
		{
			CurrentPatrolIndex = 0;
		}
		AActor *PatrolPoint = Enemy->PatrolPoints[CurrentPatrolIndex];
		if(PatrolPoint)
		{
			CurrentTargetLocation = PatrolPoint->GetActorLocation();
			bHasTarget = true;
			AIController->MoveToLocation(CurrentTargetLocation);
			CurrentPatrolIndex =
				(CurrentPatrolIndex + 1) % Enemy->PatrolPoints.Num();
		}
	}
	else
	{
		// Random movement mode
		UNavigationSystemV1 *NavSys =
			UNavigationSystemV1::GetCurrent(AIController->GetWorld());
		if(!NavSys)
		{
			return;
		}

		FVector Origin = AIController->GetPawn()->GetActorLocation();
		FNavLocation RandomLocation;

		// Find random reachable point within 1000 units
		if(NavSys->GetRandomReachablePointInRadius(
			   Origin, 1000.0f, RandomLocation))
		{
			CurrentTargetLocation = RandomLocation.Location;
			bHasTarget = true;
			AIController->MoveToLocation(CurrentTargetLocation);
		}
	}
}
