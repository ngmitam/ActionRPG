// Copyright 2025 ngmitam. All Rights Reserved.

#include "BTTask_AttackPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "MyEnemy.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = "Attack Player";
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Get player from blackboard
	UObject *PlayerObject =
		BlackboardComp->GetValueAsObject(PlayerKey.SelectedKeyName);
	if(!PlayerObject || PlayerObject->IsDefaultSubobject()
		|| !PlayerObject->IsA(ACharacter::StaticClass()))
	{
		return EBTNodeResult::Failed;
	}
	ACharacter *PlayerCharacter = Cast<ACharacter>(PlayerObject);

	// Get attack range
	float AttackRange =
		BlackboardComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
	if(AttackRange <= 0.0f)
	{
		AttackRange = Enemy->AttackRange; // Fallback to enemy's default
	}

	// Check distance
	float Distance = FVector::Dist(
		Enemy->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if(Distance <= AttackRange + FGameConfig::GetDefault().AttackRangeTolerance)
	{
		// Check if not already attacking
		if(!Enemy->bIsAttacking)
		{
			Enemy->AttackPlayer(PlayerCharacter);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			// Already attacking, fail to prevent repeated calls
			return EBTNodeResult::Failed;
		}
	}

	return EBTNodeResult::Failed;
}
