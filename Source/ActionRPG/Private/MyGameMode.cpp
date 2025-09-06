// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyCharacter.h"

AMyGameMode::AMyGameMode()
{

    DefaultPawnClass = AMyCharacter::StaticClass();
}


