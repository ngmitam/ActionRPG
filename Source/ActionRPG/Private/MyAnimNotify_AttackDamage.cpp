// Copyright 2025 ngmitam. All Rights Reserved.

#include "MyAnimNotify_AttackDamage.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "MyAttackAbility.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "MyEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

UMyAnimNotify_AttackDamage::UMyAnimNotify_AttackDamage()
{
}

void UMyAnimNotify_AttackDamage::Notify(USkeletalMeshComponent *MeshComp,
	UAnimSequenceBase *Animation,
	const FAnimNotifyEventReference &EventReference)
{
	if(!MeshComp)
	{
		return;
	}

	AActor *OwnerActor = MeshComp->GetOwner();
	if(!OwnerActor || !OwnerActor->IsValidLowLevel())
	{
		return;
	}

	PerformTraceAndApplyDamage(OwnerActor);
}

void UMyAnimNotify_AttackDamage::PerformTraceAndApplyDamage(AActor *OwnerActor)
{
	UWorld *World = OwnerActor->GetWorld();
	if(!World)
	{
		return;
	}

	ACharacter *Character = Cast<ACharacter>(OwnerActor);
	if(!Character)
	{
		return;
	}

	UAbilitySystemComponent *ASC = nullptr;
	if(AMyCharacter *MyChar = Cast<AMyCharacter>(Character))
	{
		ASC = MyChar->GetAbilitySystem();
	}
	// Enemy doesn't use GAS for attacks

	// Calculate trace start and end
	FVector StartLocation =
		OwnerActor->GetActorLocation()
		+ OwnerActor->GetActorRotation().RotateVector(TraceOffset);
	FVector EndLocation =
		StartLocation + OwnerActor->GetActorForwardVector() * TraceLength;

	// Perform sphere trace
	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor); // Ignore self

	bool bHit = World->SweepMultiByChannel(HitResults, StartLocation,
		EndLocation, FQuat::Identity, ECC_Pawn, SphereShape, QueryParams);

	bool bSpawnedParticle = false;

	if(bHit)
	{
		for(const FHitResult &Hit : HitResults)
		{
			AActor *HitActor = Hit.GetActor();

			if(HitActor
				&& (HitActor->IsA(AMyCharacter::StaticClass())
					|| HitActor->IsA(AMyEnemy::StaticClass())))
			{

				// Apply damage
				if(AMyEnemy *Enemy = Cast<AMyEnemy>(HitActor))
				{
					// For Enemy, use simple damage
					UGameplayStatics::ApplyDamage(
						Enemy, DamageAmount, nullptr, OwnerActor, nullptr);

					// Apply knockback to prevent overlap
					if(UCharacterMovementComponent *Movement =
							Enemy->GetCharacterMovement())
					{
						FVector KnockbackDirection =
							(Enemy->GetActorLocation()
								- OwnerActor->GetActorLocation())
								.GetSafeNormal();
						Movement->AddImpulse(
							KnockbackDirection
								* FGameConfig::GetDefault().KnockbackForce,
							true);
					}
				}
				else if(AMyCharacter *Player = Cast<AMyCharacter>(HitActor))
				{
					// For Player, use GAS
					UAbilitySystemComponent *TargetASC =
						Player->GetAbilitySystem();
					if(TargetASC)
					{
						TSubclassOf<UGameplayEffect> EffectToUse =
							DamageEffectClass
								? DamageEffectClass
								: TSubclassOf<UGameplayEffect>(
									  UMyDamageEffect::StaticClass());

						FGameplayEffectSpecHandle SpecHandle =
							TargetASC->MakeOutgoingSpec(EffectToUse, 1.0f,
								FGameplayEffectContextHandle());
						if(SpecHandle.IsValid())
						{
							SpecHandle.Data->SetSetByCallerMagnitude(
								FGameplayTag::RequestGameplayTag(
									FName("Data.Damage")),
								-DamageAmount);

							TargetASC->ApplyGameplayEffectSpecToTarget(
								*SpecHandle.Data.Get(), TargetASC);
						}
					}
				}

				// Spawn particle effect if from active attack ability
				if(!bSpawnedParticle && ASC)
				{
					TArray<FGameplayAbilitySpec> Specs =
						ASC->GetActivatableAbilities();
					for(const FGameplayAbilitySpec &Spec : Specs)
					{
						if(Spec.IsActive()
							&& Spec.Ability->IsA(
								UMyAttackAbility::StaticClass()))
						{
							UMyAttackAbility *AttackAbility =
								Cast<UMyAttackAbility>(Spec.Ability);
							if(AttackAbility
								&& AttackAbility->AttackParticleEffect)
							{
								UNiagaraFunctionLibrary::SpawnSystemAtLocation(
									World, AttackAbility->AttackParticleEffect,
									Hit.Location, FRotator::ZeroRotator);
								bSpawnedParticle = true;
								break;
							}
						}
					}
				}

				break; // Apply to first hit
			}
		}
	}
}
