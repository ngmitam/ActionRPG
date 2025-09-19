// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
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

protected:
	// Blackboard key to store the player
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerKey;
};
