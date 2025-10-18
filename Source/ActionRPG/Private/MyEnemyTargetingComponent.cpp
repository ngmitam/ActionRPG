// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyEnemyTargetingComponent.h"
#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"

UMyEnemyTargetingComponent::UMyEnemyTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCameraLocked = false;
}

void UMyEnemyTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start updating nearby enemies
	UpdateNearbyEnemies(); // Initial update
	GetWorld()->GetTimerManager().SetTimer(UpdateEnemiesTimerHandle, this,
		&UMyEnemyTargetingComponent::UpdateNearbyEnemies,
		FGameConfig::GetDefault().EnemyUpdateInterval, true);
}

void UMyEnemyTargetingComponent::TickComponent(float DeltaTime,
	ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update camera lock
	if(bCameraLocked && CurrentTarget && GetOwner())
	{
		FVector Direction =
			(CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation())
				.GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();

		if(APawn *OwnerPawn = Cast<APawn>(GetOwner()))
		{
			if(AController *Controller = OwnerPawn->GetController())
			{
				Controller->SetControlRotation(TargetRotation);
			}
		}
	}
}

void UMyEnemyTargetingComponent::FocusEnemy()
{
	if(CurrentTarget)
	{
		ClearTarget();
	}
	else
	{
		// Focus nearest enemy
		if(NearbyEnemies.Num() > 0)
		{
			SetTarget(NearbyEnemies[0]);
		}
	}
}

void UMyEnemyTargetingComponent::ClearTarget()
{
	if(CurrentTarget)
	{
		AMyEnemy *OldTarget = CurrentTarget;
		CurrentTarget = nullptr;
		bCameraLocked = false;

		OldTarget->SetFocused(false);
		if(!NearbyEnemies.Contains(OldTarget))
		{
			OldTarget->SetHealthBarVisible(false);
		}
	}
}

void UMyEnemyTargetingComponent::UpdateNearbyEnemies()
{
	PreviousNearbyEnemies = NearbyEnemies;
	NearbyEnemies.Empty();

	FindNearbyEnemies();
	SortEnemiesByDistance();
	UpdateHealthBarVisibility();
	ValidateCurrentTarget();
}

void UMyEnemyTargetingComponent::FindNearbyEnemies()
{
	TArray<AActor *> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(), AMyEnemy::StaticClass(), AllEnemies);

	const float DetectionRange = FGameConfig::GetDefault().EnemyDetectionRange;
	const FVector OwnerLocation =
		GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	for(AActor *Actor : AllEnemies)
	{
		if(AMyEnemy *Enemy = Cast<AMyEnemy>(Actor))
		{
			const float Distance =
				FVector::Dist(OwnerLocation, Enemy->GetActorLocation());
			if(Distance <= DetectionRange && Enemy->IsActivated())
			{
				NearbyEnemies.Add(Enemy);
			}
		}
	}
}

void UMyEnemyTargetingComponent::SortEnemiesByDistance()
{
	const FVector OwnerLocation =
		GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	NearbyEnemies.Sort(
		[OwnerLocation](const AMyEnemy &A, const AMyEnemy &B)
		{
			const float DistA =
				FVector::DistSquared(OwnerLocation, A.GetActorLocation());
			const float DistB =
				FVector::DistSquared(OwnerLocation, B.GetActorLocation());
			return DistA < DistB;
		});
}

void UMyEnemyTargetingComponent::UpdateHealthBarVisibility()
{
	// Hide health bars for enemies that are no longer nearby
	for(AMyEnemy *Enemy : PreviousNearbyEnemies)
	{
		if(!NearbyEnemies.Contains(Enemy) && Enemy != CurrentTarget)
		{
			Enemy->SetHealthBarVisible(false);
		}
	}

	// Show health bars for nearby enemies
	for(AMyEnemy *Enemy : NearbyEnemies)
	{
		if(Enemy != CurrentTarget)
		{
			Enemy->SetHealthBarVisible(true);
		}
	}
}

void UMyEnemyTargetingComponent::ValidateCurrentTarget()
{
	if(CurrentTarget
		&& (!NearbyEnemies.Contains(CurrentTarget) || CurrentTarget->bIsDead))
	{
		ClearTarget();

		// Focus new nearest enemy if available
		if(NearbyEnemies.Num() > 0)
		{
			SetTarget(NearbyEnemies[0]);
		}
	}
}

void UMyEnemyTargetingComponent::SetTarget(AMyEnemy *NewTarget)
{
	if(CurrentTarget == NewTarget)
	{
		return;
	}

	// Clear current target if different
	if(CurrentTarget)
	{
		ClearTarget();
	}

	// Set new target
	CurrentTarget = NewTarget;
	if(CurrentTarget)
	{
		bCameraLocked = true;
		CurrentTarget->SetFocused(true);
		CurrentTarget->SetHealthBarVisible(true);
	}
	else
	{
		bCameraLocked = false;
	}
}

void UMyEnemyTargetingComponent::CycleTarget()
{
	// If no nearby enemies, clear target
	if(NearbyEnemies.Num() == 0)
	{
		ClearTarget();
		return;
	}

	// Find current target index in nearby enemies
	int32 CurrentIndex = NearbyEnemies.Find(CurrentTarget);
	if(CurrentIndex == INDEX_NONE)
	{
		// Current target not in nearby enemies, start with first enemy
		CurrentIndex = 0;
	}

	// Cycle to next enemy
	const int32 NextIndex = (CurrentIndex + 1) % NearbyEnemies.Num();
	AMyEnemy *NewTarget = NearbyEnemies[NextIndex];

	SetTarget(NewTarget);
}
