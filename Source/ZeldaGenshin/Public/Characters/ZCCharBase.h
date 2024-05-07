// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZCCharBase.generated.h"

//前置声明
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UZCLayout;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EMovementTypes {
	MT_EMAX UMETA(DisplayName = EMAX),				//缺省值，留白
	MT_Walking UMETA(DisplayName = Walking),        //地面移动状态
	MT_Exhausted UMETA(DisplayName = Exhausted),    //精力耗尽，不可跑步，冲刺，滑行
	MT_Sprinting UMETA(DisplayName = Sprinting),    //冲刺状态
	MT_Gliding UMETA(DisplayName = Gliding),		//滑翔状态
	MT_Falling UMETA(DisplayName = Falling),		//下落状态
};

UENUM(BlueprintType)
enum class ERunes
{
	R_EMAX UMETA(DisplayName = EMAX),				// 缺省值，留白
	R_RBS UMETA(DisplayName = RBS),					// 遥控炸弹（球形）
	R_RBB UMETA(DisplayName = RBB),					// 遥控炸弹（方形）
	R_Mag UMETA(DisplayName = Magnesis),			// 磁铁吸附
	R_Stasis UMETA(DisplayName = Stasis),			// 时停
	R_Ice UMETA(DisplayName = Ice)					// 水面生成冰柱
};

UCLASS()
class ZELDAGENSHIN_API AZCCharBase : public ACharacter
{
	GENERATED_BODY()

public:

	AZCCharBase();


	UPROPERTY(EditAnywhere, Category = "Comps")
	USpringArmComponent* CameraBoom;
	//TObjectPtr<USpringArmComponent> CameraBoom; 等价

	UPROPERTY(EditAnywhere, Category = "Comps")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Comps")
	USkeletalMeshComponent* Parachutte;

	UPROPERTY(EditAnywhere,Category = "Inputs")
	UInputMappingContext* IMC_ZC;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* JumpGlideAction;
	
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* ToggleUIAction;



	UPROPERTY(EditAnywhere, Category = "Movements")
	EMovementTypes CurrentMT {EMovementTypes :: MT_EMAX };

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget>LayoutClassRef;

	UZCLayout* LayoutRef;

	UPROPERTY(EditDefaultsOnly, Category = "Movements")
	FVector EnableGlideDistance = FVector(0.0f,0.0f,300.0f);

	UPROPERTY(EditAnywhere, Category = "Inputs")
	bool bReadyToThrow = false;

	UPROPERTY(EditAnywhere, Category = "Runes")
	ERunes ActiveRune{ ERunes::R_EMAX };

	float Vel_X;
	float Vel_Y;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

#pragma region Inputs Node
	UFUNCTION()
	void Move_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Move_Completed(const FInputActionValue& val);

	UFUNCTION()
	void Look_Triggered(const FInputActionValue& val);

#pragma endregion
#pragma region Sprint Node
	UFUNCTION()
	void Sprint_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Sprint_Started(const FInputActionValue& val);

	UFUNCTION()
	void Sprint_Completed(const FInputActionValue& val);
#pragma endregion

#pragma region Jump & Glide Node
	UFUNCTION()
	void JumpGlide_Started(const FInputActionValue& val);

	UFUNCTION()
	void JumpGlide_Completed(const FInputActionValue& val);
#pragma endregion

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Locamotions
	UFUNCTION()
	void LocomotionManager(EMovementTypes NewMovement);

	void ResetToWalk();

	void SetSprinting();

	void SetExhausted();

	void SetGliding();

	void SetWalking();

	void SetFalling();

	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool const IsCharacterExhausted();
#pragma endregion


#pragma region Stamina
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurStamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDepletionRate = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDepletionAmount = 1.0f;


	void DrainStaminaTimer();

	FTimerHandle DrainStaminaTimerHandle;

	void StartDrainingStamina();

	void RecoverStaminaTimer();

	FTimerHandle RecoverStaminaTimerHandle;

	void StartRecoveringStamina();

	void ClearDrainRecoverTimers();

	FTimerHandle AddGravityForFlyingTimerHandle;

	void  AddGravityForFlying();
#pragma endregion
};


