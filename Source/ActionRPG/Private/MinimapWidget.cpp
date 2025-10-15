#include "MinimapWidget.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CanvasPanelSlot.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(MinimapImage && RenderTarget)
	{
		MinimapImage->SetBrushResourceObject(RenderTarget);
	}
}

void UMinimapWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePlayerMarker();
}

void UMinimapWidget::UpdatePlayerMarker()
{
	if(!PlayerMarker || !MinimapImage)
		return;

	ACharacter *PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if(!PlayerCharacter)
		return;

	// Always place player marker at center of minimap
	FVector2D MarkerPosition =
		WidgetSize / 2.0f
		- FVector2D(8.0f, 8.0f); // Center and offset for 16x16 marker

	if(UCanvasPanelSlot *CanvasSlot =
			Cast<UCanvasPanelSlot>(PlayerMarker->Slot))
	{
		CanvasSlot->SetPosition(MarkerPosition);
	}

	// No rotation needed here, handled by capture actor
}
