// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "Net/UnrealNetwork.h"

UMyAttributeSet::UMyAttributeSet()
{
	// Initialize default values for attributes
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	InitBaseDamage(10.0f);
	InitMaxWalkSpeed(300.0f); // Default walk speed
	InitStunDuration(0.0f);
}

void UMyAttributeSet::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, MaxWalkSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(
		UMyAttributeSet, StunDuration, COND_None, REPNOTIFY_Always);
}

void UMyAttributeSet::PostGameplayEffectExecute(
	const FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Check if the attribute being modified is Health.
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float OldHealth = GetHealth();
		// Ensure MaxHealth is valid
		float MaxHealthValue = GetMaxHealth();
		if(MaxHealthValue <= 0.0f)
		{
			MaxHealthValue = 100.0f; // Default value
			SetMaxHealth(MaxHealthValue);
		}
		// Clamp Health between 0 and MaxHealth.
		float NewHealth = FMath::Clamp(GetHealth(), 0.0f, MaxHealthValue);
		SetHealth(NewHealth);

		if(NewHealth <= 0.0f)
		{
			// Death
			AActor *Owner = GetOwningActor();
			AMyBaseCharacter *Character = Cast<AMyBaseCharacter>(Owner);
			if(Character && !Character->bIsDead)
			{
				Character->HandleDeath();
			}
		}
	}

	// Check if the attribute being modified is Stamina.
	if(Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Ensure MaxStamina is valid
		float MaxStaminaValue = GetMaxStamina();
		if(MaxStaminaValue <= 0.0f)
		{
			MaxStaminaValue = 100.0f; // Default value
			SetMaxStamina(MaxStaminaValue);
		}
		// Clamp Stamina between 0 and MaxStamina.
		float NewStamina = FMath::Clamp(GetStamina(), 0.0f, MaxStaminaValue);
		SetStamina(NewStamina);
	}

	// Check if the attribute being modified is StunDuration.
	if(Data.EvaluatedData.Attribute == GetStunDurationAttribute())
	{
		// Clamp StunDuration to 0 or positive
		float NewStunDuration = FMath::Max(GetStunDuration(), 0.0f);
		SetStunDuration(NewStunDuration);

		// If stunned, set timer to reset after duration
		if(NewStunDuration > 0.0f)
		{
			AActor *Owner = GetOwningActor();
			if(Owner)
			{
				FTimerHandle StunTimer;
				Owner->GetWorld()->GetTimerManager().SetTimer(
					StunTimer, [this]() { SetStunDuration(0.0f); },
					NewStunDuration, false);
			}
		}
	}

	// Handle damage if effect is damage effect
	// Removed, now handled by modifier
}

void UMyAttributeSet::OnRep_MaxWalkSpeed(
	const FGameplayAttributeData &OldMaxWalkSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxWalkSpeed, OldMaxWalkSpeed);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData &OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(
	const FGameplayAttributeData &OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_Stamina(const FGameplayAttributeData &OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Stamina, OldStamina);
}

void UMyAttributeSet::OnRep_MaxStamina(
	const FGameplayAttributeData &OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxStamina, OldMaxStamina);
}

void UMyAttributeSet::OnRep_BaseDamage(
	const FGameplayAttributeData &OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, BaseDamage, OldBaseDamage);
}

void UMyAttributeSet::OnRep_StunDuration(
	const FGameplayAttributeData &OldStunDuration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, StunDuration, OldStunDuration);

	// Broadcast delegate for stun duration changes
	OnStunDurationChanged.Broadcast(GetStunDuration());
}
