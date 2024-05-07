// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZCRuneSelections.h"

void UZCRuneSelections::SelectedRuneType(TEnumAsByte<ERunes> RuneType)
{
	if (PlayerRef)
	{
		PlayerRef->ActiveRune = RuneType;
	}
}