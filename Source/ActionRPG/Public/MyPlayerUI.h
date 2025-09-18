// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "MyPlayerUI.generated.h"

class UProgressBar;
class UTextBlock;
class AMyCharacter;

/**
 * Player UI widget that displays health and stamina
 */
UCLASS(ClassGroup = (UI), meta = (BlueprintSpawnableComponent, Blueprintable,
                                  DisplayName = "My Player UI"))
class ACTIONRPG_API UMyPlayerUI : public UUserWidget {
  GENERATED_BODY()

public:
  UMyPlayerUI(const FObjectInitializer &ObjectInitializer);

  virtual void NativeConstruct() override;
  virtual void NativeTick(const FGeometry &MyGeometry,
                          float InDeltaTime) override;

  // Update UI elements
  void UpdateHealthUI();
  void UpdateStaminaUI();
  void UpdateAllUI();

  // Set the owning character
  void SetOwningCharacter(AMyCharacter *InCharacter) {
    OwningCharacter = InCharacter;
  }

  // Blueprint-callable functions for UI updates
  UFUNCTION(BlueprintCallable, Category = "UI")
  void UpdateHealthDisplay();

  UFUNCTION(BlueprintCallable, Category = "UI")
  void UpdateStaminaDisplay();

  UFUNCTION(BlueprintCallable, Category = "UI")
  void UpdateAllDisplays();

protected:
  // UI Elements
  UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI Elements")
  UProgressBar *HealthBar;

  UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI Elements")
  UProgressBar *StaminaBar;

private:
  // Reference to the owning character
  AMyCharacter *OwningCharacter;
};
