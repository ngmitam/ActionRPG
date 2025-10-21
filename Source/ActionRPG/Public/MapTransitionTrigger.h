// Copyright 2025 ngmitam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapTransitionTrigger.generated.h"

UCLASS()
class ACTIONRPG_API AMapTransitionTrigger : public AActor
{
	GENERATED_BODY()

public:
	AMapTransitionTrigger();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class UBoxComponent *TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Transition")
	FName TargetMapName;

private:
	bool bActivated = false;
};
