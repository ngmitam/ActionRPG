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
	virtual void NativeConstruct() override;
	virtual void NativeTick(
		const FGeometry &MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UImage *MinimapImage;

	UPROPERTY(meta = (BindWidget))
	UImage *PlayerMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	UTextureRenderTarget2D *RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MapSize = 5000.0f; // World units covered by minimap

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D WidgetSize = FVector2D(256.0f, 256.0f); // Size of minimap widget

private:
	void UpdatePlayerMarker();
};
