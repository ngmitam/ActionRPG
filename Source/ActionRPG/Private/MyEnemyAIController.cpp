// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEnemyAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyEnemy.h"

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

		// Set attack range in blackboard for dynamic behavior
		AMyEnemy *Enemy = Cast<AMyEnemy>(InPawn);
		if(Enemy)
		{
			BlackboardComponent->SetValueAsFloat(
				FName("AttackRange"), Enemy->AttackRange);
		}

		// Set patrol points in blackboard
		if(Enemy && Enemy->PatrolPoints.Num() > 0)
		{
			BlackboardComponent->SetValueAsVector(FName("PatrolLocation"),
				Enemy->PatrolPoints[0]->GetActorLocation());
		}

		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
	else
	{
	}
}

void AMyEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	BehaviorTreeComponent->StopTree();
}

void AMyEnemyAIController::AlertNearbyEnemies()
{
	TArray<AActor *> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(), AMyEnemyAIController::StaticClass(), FoundActors);

	for(AActor *Actor : FoundActors)
	{
		AMyEnemyAIController *OtherController =
			Cast<AMyEnemyAIController>(Actor);
		if(OtherController && OtherController != this)
		{
			float Distance = FVector::Dist(GetPawn()->GetActorLocation(),
				OtherController->GetPawn()->GetActorLocation());
			if(Distance <= AlertRange)
			{
				// Set alerted in blackboard
				if(OtherController->BlackboardComponent)
				{
					OtherController->BlackboardComponent->SetValueAsBool(
						FName("Alerted"), true);
				}
			}
		}
	}
}
