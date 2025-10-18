#include "MinimapWidget.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

void UMinimapWidget::SetRenderTarget(UTextureRenderTarget2D *InRenderTarget)
{
	if(MinimapImage && InRenderTarget)
	{
		MinimapImage->SetBrushResourceObject(InRenderTarget);
	}
}
