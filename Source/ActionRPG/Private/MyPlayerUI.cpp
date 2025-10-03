// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyPlayerUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyAbilityTypes.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"

UMyPlayerUI::UMyPlayerUI(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	OwningCharacter = nullptr;
}

void UMyPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Initial UI update
	UpdateAllUI();
}

void UMyPlayerUI::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Update UI every frame
	UpdateAllUI();
}

void UMyPlayerUI::UpdateHealthDisplay()
{
	UpdateHealthUI();
}

void UMyPlayerUI::UpdateStaminaDisplay()
{
	UpdateStaminaUI();
}

void UMyPlayerUI::UpdateAllDisplays()
{
	UpdateAllUI();
}

void UMyPlayerUI::UpdateHealthUI()
{
	if(!OwningCharacter)
	{
		return;
	}

	UMyAttributeComponent *AttrComp = OwningCharacter->GetAttributeComponent();
	if(!AttrComp)
	{
		return;
	}

	float CurrentHealth = AttrComp->GetHealth();
	float MaxHealth = AttrComp->GetMaxHealth();

	// Update progress bar
	if(HealthBar)
	{
		float HealthPercent =
			(MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
		HealthBar->SetPercent(HealthPercent);

		// Change color based on health level
		if(HealthPercent > UIConstants::HealthHighThreshold)
		{
			HealthBar->SetFillColorAndOpacity(UIConstants::HealthColorHigh);
		}
		else if(HealthPercent > UIConstants::HealthMediumThreshold)
		{
			HealthBar->SetFillColorAndOpacity(UIConstants::HealthColorMedium);
		}
		else
		{
			HealthBar->SetFillColorAndOpacity(UIConstants::HealthColorLow);
		}
	}
}

void UMyPlayerUI::UpdateStaminaUI()
{
	if(!OwningCharacter)
	{
		return;
	}

	UMyAttributeComponent *AttrComp = OwningCharacter->GetAttributeComponent();
	if(!AttrComp)
	{
		return;
	}

	float CurrentStamina = AttrComp->GetStamina();
	float MaxStamina = AttrComp->GetMaxStamina();

	// Update progress bar
	if(StaminaBar)
	{
		float StaminaPercent =
			(MaxStamina > 0.0f) ? (CurrentStamina / MaxStamina) : 0.0f;
		StaminaBar->SetPercent(StaminaPercent);

		// Change color based on stamina level
		if(StaminaPercent > UIConstants::StaminaHighThreshold)
		{
			StaminaBar->SetFillColorAndOpacity(UIConstants::StaminaColorHigh);
		}
		else if(StaminaPercent > UIConstants::StaminaMediumThreshold)
		{
			// Use a custom orange color since FLinearColor::Orange doesn't
			// exist
			StaminaBar->SetFillColorAndOpacity(UIConstants::StaminaColorMedium);
		}
		else
		{
			StaminaBar->SetFillColorAndOpacity(UIConstants::StaminaColorLow);
		}
	}
}

void UMyPlayerUI::UpdateAllUI()
{
	UpdateHealthUI();
	UpdateStaminaUI();
}
