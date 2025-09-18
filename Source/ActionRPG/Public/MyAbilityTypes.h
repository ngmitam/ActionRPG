// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EMyAbilityInputID : uint8 {
  None UMETA(DisplayName = "None"),
  Sprint UMETA(DisplayName = "Sprint"),
  Jump UMETA(DisplayName = "Jump"),
  Dodge UMETA(DisplayName = "Dodge"),
  Attack UMETA(DisplayName = "Attack")
};
