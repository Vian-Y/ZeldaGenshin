// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZELDAGENSHIN_API AZCPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	virtual void BeginPlay() override;
};
