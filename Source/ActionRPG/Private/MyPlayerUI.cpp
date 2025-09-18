// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"

UMyPlayerUI::UMyPlayerUI(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer) {
  OwningCharacter = nullptr;
}

void UMyPlayerUI::NativeConstruct() {
  Super::NativeConstruct();

  // Initial UI update
  UpdateAllUI();
}

void UMyPlayerUI::NativeTick(const FGeometry &MyGeometry, float InDeltaTime) {
  Super::NativeTick(MyGeometry, InDeltaTime);

  // Update UI every frame
  UpdateAllUI();
}

void UMyPlayerUI::UpdateHealthDisplay() { UpdateHealthUI(); }

void UMyPlayerUI::UpdateStaminaDisplay() { UpdateStaminaUI(); }

void UMyPlayerUI::UpdateAllDisplays() { UpdateAllUI(); }

void UMyPlayerUI::UpdateHealthUI() {
  if (!OwningCharacter) {
    return;
  }

  UMyAttributeComponent *AttrComp = OwningCharacter->GetAttributeComponent();
  if (!AttrComp) {
    return;
  }

  float CurrentHealth = AttrComp->GetHealth();
  float MaxHealth = AttrComp->GetMaxHealth();

  // Update progress bar
  if (HealthBar) {
    float HealthPercent =
        (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
    HealthBar->SetPercent(HealthPercent);

    // Change color based on health level
    if (HealthPercent > 0.6f) {
      HealthBar->SetFillColorAndOpacity(FLinearColor::Green);
    } else if (HealthPercent > 0.3f) {
      HealthBar->SetFillColorAndOpacity(FLinearColor::Yellow);
    } else {
      HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
    }
  }
}

void UMyPlayerUI::UpdateStaminaUI() {
  if (!OwningCharacter) {
    return;
  }

  UMyAttributeComponent *AttrComp = OwningCharacter->GetAttributeComponent();
  if (!AttrComp) {
    return;
  }

  float CurrentStamina = AttrComp->GetStamina();
  float MaxStamina = AttrComp->GetMaxStamina();

  // Update progress bar
  if (StaminaBar) {
    float StaminaPercent =
        (MaxStamina > 0.0f) ? (CurrentStamina / MaxStamina) : 0.0f;
    StaminaBar->SetPercent(StaminaPercent);

    // Change color based on stamina level
    if (StaminaPercent > 0.5f) {
      StaminaBar->SetFillColorAndOpacity(FLinearColor::Blue);
    } else if (StaminaPercent > 0.2f) {
      // Use a custom orange color since FLinearColor::Orange doesn't exist
      StaminaBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.5f, 0.0f, 1.0f));
    } else {
      StaminaBar->SetFillColorAndOpacity(FLinearColor::Red);
    }
  }
}

void UMyPlayerUI::UpdateAllUI() {
  UpdateHealthUI();
  UpdateStaminaUI();
}
