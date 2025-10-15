// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoreMinimal.h"
#include "MyEnemyAIController.generated.h"

/**
 * AI Controller for MyEnemy
 */
UCLASS()
class ACTIONRPG_API AMyEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyEnemyAIController();

	virtual void OnPossess(APawn *InPawn) override;
	virtual void OnUnPossess() override;

	// Detection range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRange = 500.0f;

protected:
	virtual void BeginPlay() override;

private:
	// Behavior Tree to run
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree *BehaviorTree;

	// Behavior Tree Component
	UPROPERTY()
	UBehaviorTreeComponent *BehaviorTreeComponent;

	// Blackboard Component
	UPROPERTY()
	UBlackboardComponent *BlackboardComponent;
};
