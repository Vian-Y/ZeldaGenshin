// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ZCPlayerController.h"

void AZCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
}
