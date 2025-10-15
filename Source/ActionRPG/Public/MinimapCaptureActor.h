#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapCaptureActor.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class ACTIONRPG_API AMinimapCaptureActor : public AActor
{
	GENERATED_BODY()

public:
	AMinimapCaptureActor();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	USceneCaptureComponent2D *SceneCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	UTextureRenderTarget2D *RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float CaptureHeight = 1000.0f; // Height above ground to capture

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float UpdateInterval = 0.1f; // Update every 0.1 seconds for performance

private:
	FTimerHandle UpdateTimerHandle;

	void UpdateCapture();
};
