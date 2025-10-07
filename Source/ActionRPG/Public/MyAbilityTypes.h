// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "MyAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EMyAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Sprint UMETA(DisplayName = "Sprint"),
	Jump UMETA(DisplayName = "Jump"),
	Dodge UMETA(DisplayName = "Dodge"),
	Attack UMETA(DisplayName = "Attack")
};

// Gameplay Tag Constants
namespace AbilityTags
{
const FName Sprint = FName("Ability.Sprint");
const FName Jump = FName("Ability.Jump");
const FName Dodge = FName("Ability.Dodge");
const FName Attack = FName("Ability.Attack");
} // namespace AbilityTags

namespace StateTags
{
const FName Sprinting = FName("State.Sprinting");
}

// Component Name Constants
namespace ComponentNames
{
const FName SpringArm = FName("SpringArm");
const FName Camera = FName("Camera");
const FName AttributeComponent = FName("AttributeComponent");
const FName AbilitySystemComponent = FName("AbilitySystemComponent");
const FName HealthBarWidget = FName("HealthBarWidget");
} // namespace ComponentNames