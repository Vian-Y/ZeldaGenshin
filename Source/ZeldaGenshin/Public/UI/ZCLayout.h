// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/ZCCharBase.h"
#include "ZCLayout.generated.h"

/**
 * 
 */
UCLASS()
class ZELDAGENSHIN_API UZCLayout : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDeferred(AZCCharBase* PlayerRef);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGaugeAnim(bool bShow);
};
