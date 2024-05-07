// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


namespace Debug
{
	static void Print(const FString& DebugMessage)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMessage);
		}
	}
}
	