// Copyright 2025 ngmitam. All Rights Reserved.

#include "MapTransitionTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"

AMapTransitionTrigger::AMapTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this, &AMapTransitionTrigger::OnOverlapBegin);
}

void AMapTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void AMapTransitionTrigger::OnOverlapBegin(UPrimitiveComponent *OverlappedComp,
	AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult)
{
	if(bActivated)
		return;

	if(!OtherActor || OtherActor->HasAnyFlags(RF_ClassDefaultObject))
		return;

	AMyCharacter *Player = Cast<AMyCharacter>(OtherActor);
	if(Player && !TargetMapName.IsNone())
	{
		// Transition to the target map
		UGameplayStatics::OpenLevel(this, TargetMapName);

		bActivated = true;
	}
}
