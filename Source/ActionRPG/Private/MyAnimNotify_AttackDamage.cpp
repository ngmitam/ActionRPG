// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimNotify_AttackDamage.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "MyEnemy.h"

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
	if(!OwnerActor)
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
	else if(AMyEnemy *Enemy = Cast<AMyEnemy>(Character))
	{
		ASC = Enemy->GetAbilitySystem();
	}
	if(!ASC)
	{
		return;
	}

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
				TSubclassOf<UGameplayEffect> EffectToUse =
					DamageEffectClass ? DamageEffectClass
									  : TSubclassOf<UGameplayEffect>(
											UMyDamageEffect::StaticClass());

				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
					EffectToUse, 1.0f, FGameplayEffectContextHandle());
				if(SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
						DamageAmount);

					UAbilitySystemComponent *TargetASC = nullptr;
					if(AMyCharacter *Char = Cast<AMyCharacter>(HitActor))
					{
						TargetASC = Char->GetAbilitySystem();
					}
					else if(AMyEnemy *Enemy = Cast<AMyEnemy>(HitActor))
					{
						TargetASC = Enemy->GetAbilitySystem();
					}

					if(TargetASC)
					{
						ASC->ApplyGameplayEffectSpecToTarget(
							*SpecHandle.Data.Get(), TargetASC);
					}
				}
				break; // Apply to first hit
			}
		}
	}
}
