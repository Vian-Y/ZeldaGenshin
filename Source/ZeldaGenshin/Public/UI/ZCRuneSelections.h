// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/ZCCharBase.h"
#include "ZCRuneSelections.generated.h"

/**
 * 
 */
UCLASS()
class ZELDAGENSHIN_API UZCRuneSelections : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AZCCharBase* PlayerRef;
	
	UFUNCTION(BlueprintCallable, Category = "Test")
	void SelectedRuneType(TEnumAsByte<ERunes> RuneType);
};
