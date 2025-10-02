// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossAreaTrigger.generated.h"

class AMyBoss;

UCLASS()
class ACTIONRPG_API ABossAreaTrigger : public AActor
{
	GENERATED_BODY()

public:
	ABossAreaTrigger();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class UBoxComponent *TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	AMyBoss *BossToActivate;

private:
	bool bActivated = false;
};
