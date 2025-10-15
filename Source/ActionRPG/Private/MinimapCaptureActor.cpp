#include "MinimapCaptureActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AMinimapCaptureActor::AMinimapCaptureActor()
{
	PrimaryActorTick.bCanEverTick = false; // No need to tick for performance

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(
		TEXT("SceneCaptureComponent"));
	RootComponent = SceneCaptureComponent;

	// Set up scene capture for top-down view
	SceneCaptureComponent->bCaptureEveryFrame =
		false; // Manual capture for performance
	SceneCaptureComponent->bCaptureOnMovement = false;
	SceneCaptureComponent->CaptureSource =
		ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCaptureComponent->bAlwaysPersistRenderingState = true;

	// Set projection type to orthographic for minimap
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent->OrthoWidth = 5000.0f; // Adjust based on map size

	// Rotation will be set in UpdateCapture
}

void AMinimapCaptureActor::BeginPlay()
{
	Super::BeginPlay();

	if(RenderTarget)
	{
		SceneCaptureComponent->TextureTarget = RenderTarget;
	}

	// Start update timer
	GetWorldTimerManager().SetTimer(UpdateTimerHandle, this,
		&AMinimapCaptureActor::UpdateCapture, UpdateInterval, true);
}

void AMinimapCaptureActor::UpdateCapture()
{
	if(SceneCaptureComponent && RenderTarget)
	{
		// Follow player position
		APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if(PlayerPawn)
		{
			FVector PlayerLocation = PlayerPawn->GetActorLocation();
			SetActorLocation(
				FVector(PlayerLocation.X, PlayerLocation.Y, CaptureHeight));

			// Set capture rotation to top-down with player yaw adjusted so
			// player direction is "up" on map
			FRotator PlayerRotation = PlayerPawn->GetActorRotation();
			float AdjustedYaw =
				PlayerRotation.Yaw; // Adjust so player forward becomes map up
			SceneCaptureComponent->SetWorldRotation(
				FRotator(-90.0f, AdjustedYaw, 0.0f));
		}

		SceneCaptureComponent->CaptureScene();
	}
}
