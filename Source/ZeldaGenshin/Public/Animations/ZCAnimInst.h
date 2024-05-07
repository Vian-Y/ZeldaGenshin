// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/ZCCharBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZCAnimInst.generated.h"

class AZCCharBase;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ZELDAGENSHIN_API UZCAnimInst : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation()override;
	virtual void NativeUpdateAnimation(float DeltaTime)override;

	UPROPERTY()
	AZCCharBase* PlayerRef;

	UPROPERTY()
	UCharacterMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	float GroundSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	float AirSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bShouldMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bIsGliding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool bReadyToThrow = false;

};
