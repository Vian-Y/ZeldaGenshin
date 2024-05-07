// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/ZCCharBase.h"
#include "WindTunnel.generated.h"


class AZCCharBase;
class UBoxComponent;

UCLASS()
class ZELDAGENSHIN_API AWindTunnel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindTunnel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool bTemporaryWT = false;

	UPROPERTY(EditAnywhere)
	AZCCharBase* PlayerRef = nullptr;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
