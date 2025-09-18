// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimNotify_ComboWindowStart.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "MyAttackAbility.h"
#include "MyCharacter.h"

UMyAnimNotify_ComboWindowStart::UMyAnimNotify_ComboWindowStart() {}

void UMyAnimNotify_ComboWindowStart::Notify(USkeletalMeshComponent *MeshComp,
                                            UAnimSequenceBase *Animation) {
  if (!MeshComp) {
    return;
  }

  ACharacter *Character = Cast<ACharacter>(MeshComp->GetOwner());
  if (!Character) {
    return;
  }

  AMyCharacter *MyCharacter = Cast<AMyCharacter>(Character);
  if (!MyCharacter) {
    return;
  }

  // Get the current attack ability and start combo window
  if (UAbilitySystemComponent *ASC = MyCharacter->GetAbilitySystem()) {
    // Find the active attack ability
    TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();
    for (const FGameplayAbilitySpec &Spec : Abilities) {
      if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(
                              UMyAttackAbility::StaticClass())) {
        if (Spec.IsActive()) {
          UMyAttackAbility *AttackAbility =
              Cast<UMyAttackAbility>(Spec.GetPrimaryInstance());
          if (AttackAbility) {
            // Start combo window (duration controlled by EndComboWindow notify)
            AttackAbility->StartComboWindow();
            break;
          }
        }
      }
    }
  }
}
