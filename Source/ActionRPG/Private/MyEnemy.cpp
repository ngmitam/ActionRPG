// Fill out your copyright notice in the Description page of Project Settings.

#include "MyEnemy.h"
#include "MyAttributeComponent.h"
#include "MyCharacter.h"
#include "MyDamageEffect.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

AMyEnemy::AMyEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    // Simple movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AMyEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Find player
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

    // Initialize AttributeComponent
    if (!AttributeComponent)
    {
        // Assume AttributeComponentClass is set, but for simplicity, create default
        AttributeComponent = NewObject<UMyAttributeComponent>(this);
        if (AttributeComponent)
        {
            AttributeComponent->RegisterComponent();
            AttributeComponent->InitializeAbilitySystem();
        }
    }
}

void AMyEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter)
    {
        MoveTowardsPlayer(DeltaTime);
        AttackPlayer();
    }
}

void AMyEnemy::MoveTowardsPlayer(float DeltaTime)
{
    FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
    Direction.Z = 0.0f; // Keep on ground
    Direction.Normalize();

    // Simple move towards player
    AddMovementInput(Direction, 1.0f);
}

void AMyEnemy::AttackPlayer()
{
    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance <= AttackRange && GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown)
    {
        LastAttackTime = GetWorld()->GetTimeSeconds();

        // Apply damage to player
        if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent())
        {
            AMyCharacter *Player = Cast<AMyCharacter>(PlayerCharacter);
            if (Player && Player->GetAttributeComponent() && Player->GetAbilitySystem())
            {
                // Create damage effect spec
                FGameplayEffectSpecHandle DamageSpecHandle = AttributeComponent->GetAbilitySystemComponent()->MakeOutgoingSpec(UMyDamageEffect::StaticClass(), 1.0f, FGameplayEffectContextHandle());
                if (DamageSpecHandle.IsValid())
                {
                    // Set damage magnitude (negative for damage)
                    DamageSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -AttackDamage);

                    // Apply to player
                    AttributeComponent->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), Player->GetAbilitySystem());
                }
            }
        }
    }
}

UAbilitySystemComponent *AMyEnemy::GetAbilitySystem() const
{
    return AttributeComponent ? AttributeComponent->GetAbilitySystemComponent() : nullptr;
}

void AMyEnemy::HandleDeath()
{
    // Disable tick
    SetActorTickEnabled(false);

    // Disable movement
    GetCharacterMovement()->DisableMovement();

    // Stop any abilities
    if (AttributeComponent && AttributeComponent->GetAbilitySystemComponent())
    {
        AttributeComponent->GetAbilitySystemComponent()->CancelAllAbilities();
    }

    // Play death animation
    if (DeathMontage)
    {
        USkeletalMeshComponent *SkeletalMesh = GetMesh();
        if (SkeletalMesh)
        {
            UAnimInstance *AnimInstance = SkeletalMesh->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(DeathMontage);
                // Set timer to destroy after animation
                float Duration = AnimInstance->Montage_GetPlayRate(DeathMontage) > 0.0f
                                     ? DeathMontage->GetPlayLength() / AnimInstance->Montage_GetPlayRate(DeathMontage)
                                     : DeathMontage->GetPlayLength();
                FTimerHandle DeathTimer;
                GetWorld()->GetTimerManager().SetTimer(DeathTimer, [this]()
                                                       {
                    if (AttributeComponent)
                    {
                        AttributeComponent->HandleDeath();
                    } }, Duration, false);
                return; // Don't destroy immediately
            }
        }
    }

    // Fallback: destroy after delay
    if (AttributeComponent)
    {
        AttributeComponent->HandleDeath();
    }
}
