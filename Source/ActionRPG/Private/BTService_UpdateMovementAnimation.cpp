// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_UpdateMovementAnimation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyEnemy.h"

UBTService_UpdateMovementAnimation::UBTService_UpdateMovementAnimation()
{
	NodeName = "Update Movement Animation";
	// Run every frame
	Interval = 0.0f;
	bNotifyTick = true;
}

void UBTService_UpdateMovementAnimation::TickNode(
	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController *AIController = OwnerComp.GetAIOwner();
	if(!AIController)
	{
		return;
	}

	AMyEnemy *Enemy = Cast<AMyEnemy>(AIController->GetPawn());
	if(!Enemy)
	{
		return;
	}

	// Check if moving
	bool bIsMoving = Enemy->GetVelocity().Size() > 0.0f;
	Enemy->PlayMovementAnimation(bIsMoving);
}
