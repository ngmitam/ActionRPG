// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
#include "MyEnemyAIController.h"
#include "BTTask_FindPlayer.generated.h"

/**
 * BT Task to find the player and store in Blackboard
 */
UCLASS()
class ACTIONRPG_API UBTTask_FindPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPlayer();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory,
		float DeltaSeconds) override;

protected:
	// Blackboard key to store the player
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerKey;

private:
	void StartRandomMovement(AAIController *AIController);

	FVector CurrentTargetLocation;
	bool bHasTarget;
};
