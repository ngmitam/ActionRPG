#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UImage;
class UCanvasPanelSlot;

UCLASS()
class ACTIONRPG_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage *MinimapImage;

	UPROPERTY(meta = (BindWidget))
	UImage *PlayerMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D WidgetSize = FVector2D(256.0f, 256.0f); // Size of minimap widget

public:
	void SetRenderTarget(UTextureRenderTarget2D *InRenderTarget);

private:
	void UpdatePlayerMarker();
};
