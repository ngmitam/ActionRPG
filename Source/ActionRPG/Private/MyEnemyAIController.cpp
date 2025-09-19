// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEnemyAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyEnemyAIController::AMyEnemyAIController()
{
	// Create Behavior Tree Component
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(
		TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(
		TEXT("BlackboardComponent"));
}

void AMyEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyEnemyAIController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);

	if(BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(
			*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
}

void AMyEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	BehaviorTreeComponent->StopTree();
}
