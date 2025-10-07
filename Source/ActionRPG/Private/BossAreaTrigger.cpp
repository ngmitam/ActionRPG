// Copyright 2025 ngmitam. All Rights Reserved.

#include "BossAreaTrigger.h"

#include "Components/BoxComponent.h"
#include "MyBoss.h"
#include "MyCharacter.h"

ABossAreaTrigger::ABossAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this, &ABossAreaTrigger::OnOverlapBegin);
}

void ABossAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ABossAreaTrigger::OnOverlapBegin(UPrimitiveComponent *OverlappedComp,
	AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
{
	if(bActivated)
		return;

	if(!OtherActor || OtherActor->HasAnyFlags(RF_ClassDefaultObject))
		return;

	AMyCharacter *Player = Cast<AMyCharacter>(OtherActor);
	if(Player && BossToActivate.IsValid() && BossToActivate.Get()
		&& BossToActivate.Get()->IsValidLowLevel()
		&& !BossToActivate.Get()->IsTemplate()
		&& !BossToActivate.Get()->HasAnyFlags(RF_ClassDefaultObject))
	{
		// Activate boss
		BossToActivate.Get()->SetActorHiddenInGame(false);
		BossToActivate.Get()->SetActorEnableCollision(true);

		// Optional: Play activation animation or sound
		// BossToActivate->PlayActivationSequence();

		bActivated = true;
	}
}
