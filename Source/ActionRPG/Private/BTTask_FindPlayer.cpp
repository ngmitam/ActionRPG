// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_FindPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindPlayer::UBTTask_FindPlayer()
{
	NodeName = "Find Player";
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
		return EBTNodeResult::Failed;
	}

	// Set player in blackboard
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(
			PlayerKey.SelectedKeyName, PlayerCharacter);

		// Alert nearby enemies
		EnemyController->AlertNearbyEnemies();

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
