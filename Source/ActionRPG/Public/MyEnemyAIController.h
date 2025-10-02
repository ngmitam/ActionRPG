// Fill out your copyright notice in the Description page of Project Settings.

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

	// Alert range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AlertRange = 1000.0f;

	// Alert nearby enemies
	void AlertNearbyEnemies();

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
