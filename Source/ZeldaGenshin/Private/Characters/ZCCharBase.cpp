// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ZCCharBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/ZCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Debug/DebugHelper.h"
#include "UI/ZCLayout.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AZCCharBase::AZCCharBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Parachutte = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Parachutte"));
	Parachutte->SetupAttachment(GetMesh());
	Parachutte->SetVisibility(false);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

// Called when the game starts or when spawned
void AZCCharBase::BeginPlay()
{
	Super::BeginPlay();
	AZCPlayerController* PC = Cast<AZCPlayerController>(Controller);
	if (PC == nullptr) return; //CastFailed

	UEnhancedInputLocalPlayerSubsystem* Subsystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (Subsystem == nullptr) return;
	Subsystem->AddMappingContext(IMC_ZC,0);

	//��ʼ������ֵ
	CurStamina = MaxStamina;

	//����UI
	if (LayoutClassRef) 
	{
		LayoutRef = CreateWidget<UZCLayout>(GetWorld(), LayoutClassRef);
		if (LayoutRef)
		{
			LayoutRef->ConstructDeferred(this);
			LayoutRef->AddToViewport();
		}
	}
}

void AZCCharBase::Landed(const FHitResult& Hit)
{
	if (CurrentMT == EMovementTypes::MT_Exhausted)
	{
		//�ָ�����ֵ
		StartRecoveringStamina();
		return;
	}
	if (CurrentMT == EMovementTypes::MT_Gliding)
	{
		//����ڿ��У��������л�ΪFalling
		LocomotionManager(EMovementTypes::MT_Walking);
		return;
	}

	if (CurrentMT == EMovementTypes::MT_Sprinting)
	{
		//�������
		LocomotionManager(EMovementTypes::MT_Sprinting);
	}
	else
	{
		LocomotionManager(EMovementTypes::MT_Walking);
		
	}
}


#pragma region Move&Camera Node
void AZCCharBase::Move_Triggered(const FInputActionValue& val)
{
	const FVector2D InputVector = val.Get<FVector2D>();
	Vel_X = InputVector.X;
	Vel_Y = InputVector.Y;

	if (Controller == nullptr) return;
	//ֻ��עˮƽ����Yaw
	const FRotator GroundRotation(0, Controller->GetControlRotation().Yaw, 0);
	//���� Y
	const FVector RightDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, Vel_X);
	//ǰ�� X
	const FVector FwdDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(FwdDir, Vel_Y);
}

void AZCCharBase::Move_Completed(const FInputActionValue& val)
{
	Vel_X = 0;
	Vel_Y = 0;
}

void AZCCharBase::Look_Triggered(const FInputActionValue& val)
{
	FVector2D LookVal = val.Get<FVector2D>();
	if (Controller == nullptr) return;

	AddControllerYawInput(LookVal.X);
	AddControllerPitchInput(LookVal.Y);

}

#pragma endregion


#pragma region Sprint Node
void AZCCharBase::Sprint_Triggered(const FInputActionValue& val)
{
	//���ڼ����������������ڳ��״̬ʱ��ȡ�����״̬����Walking״̬
	if (Vel_Y == 0.0f && Vel_X == 0.0f && CurrentMT == EMovementTypes::MT_Sprinting)
	{
		LocomotionManager(EMovementTypes::MT_Walking);
	}
}

void AZCCharBase::Sprint_Started(const FInputActionValue& val)
{
	if (CurrentMT == EMovementTypes::MT_Walking || CurrentMT == EMovementTypes::MT_EMAX)
	{
		LocomotionManager(EMovementTypes::MT_Sprinting);
	}
}

void AZCCharBase::Sprint_Completed(const FInputActionValue& val)
{
	if (CurrentMT == EMovementTypes::MT_Sprinting)
	{
		LocomotionManager(EMovementTypes::MT_Walking);
	}
}

void AZCCharBase::JumpGlide_Started(const FInputActionValue& val)
{
	if (CurrentMT == EMovementTypes::MT_Exhausted) {
		//��bug
	
		
		return;
	}

	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		Jump();
		LocomotionManager(EMovementTypes::MT_Falling);
		//�����ú���
		return;
	}

	if (CurrentMT == EMovementTypes::MT_Gliding)
	{
		//�Ѿ��ڻ���״̬����ȡ����������½�״̬
		LocomotionManager(EMovementTypes::MT_Falling);
		//�����ú���
		return;
	}

	//����Ƿ������������������������ܽ��뻬��״̬

	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start - EnableGlideDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool HitAnything = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End,
		ECollisionChannel::ECC_Visibility,Params);
	//��ʾ����
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0.0f, 3.0f);

	if (HitAnything)
	{
		//���ɻ���
	}
	else
	{
		//TBD - ȡ�������ͷż���״̬

		//�л���Gliding����״̬
		LocomotionManager(EMovementTypes::MT_Gliding);

	}

}

void AZCCharBase::JumpGlide_Completed(const FInputActionValue& val)
{
	StopJumping();
}

#pragma endregion


// Called every frame
void AZCCharBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*FString FloatStr = FString::SanitizeFloat(CurStamina);
	Debug::Print(*FloatStr);*/
}

// Called to bind functionality to input
void AZCCharBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIComp == nullptr) return;

	EIComp->BindAction(MoveAction,ETriggerEvent::Triggered,this, &AZCCharBase::Move_Triggered);
	EIComp->BindAction(MoveAction, ETriggerEvent::Completed, this, &AZCCharBase::Move_Completed);

	EIComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &AZCCharBase::Look_Triggered);

	EIComp->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AZCCharBase::Sprint_Triggered);
	EIComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &AZCCharBase::Sprint_Completed);
	EIComp->BindAction(SprintAction, ETriggerEvent::Started, this, &AZCCharBase::Sprint_Started);

	EIComp->BindAction(JumpGlideAction, ETriggerEvent::Completed, this, &AZCCharBase::JumpGlide_Completed);
	EIComp->BindAction(JumpGlideAction, ETriggerEvent::Started, this, &AZCCharBase::JumpGlide_Started);


}


#pragma region Locomotion

void AZCCharBase::LocomotionManager(EMovementTypes NewMovement)
{
	//���Ƹ����˶�
	if (NewMovement == CurrentMT) return;
	CurrentMT = NewMovement;

	//����ڻ���״̬����ʾ������ģ��
	if(Parachutte)
	{
		//��ʾ������ģ��
		Parachutte->SetVisibility(CurrentMT == EMovementTypes::MT_Gliding);
	}

	//����ö��ִֵ�в�ͬ�˶��߼�
	switch(CurrentMT)
	{
		case EMovementTypes::MT_EMAX:
			break;
		case EMovementTypes::MT_Walking:
			SetWalking();
			
			break;
		case EMovementTypes::MT_Exhausted:
			SetExhausted();
			break;
		case EMovementTypes::MT_Sprinting:
			SetSprinting();
			break;
		case EMovementTypes::MT_Gliding:
			SetGliding();
			break;
		case EMovementTypes::MT_Falling:
			SetFalling();
			break;

	}
}

void AZCCharBase::ResetToWalk()
{
	Debug::Print(TEXT("ResetToWalk"));
	//�����������������˴�ȡ��
	GetWorldTimerManager().ClearTimer(AddGravityForFlyingTimerHandle);
	//���ûص���״̬���ӻ��裬����״̬��
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AZCCharBase::SetSprinting()
{
	Debug::Print(TEXT("Sprinting"));
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();
	//���ľ�����
	StartDrainingStamina();
}

void AZCCharBase::SetExhausted()
{
	Debug::Print(TEXT("Exhausted"));
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	ClearDrainRecoverTimers(); //���Timer��ֹͣ����

	//���������״̬�������̻ظ�����;���Ӵ�������ٻظ�����
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		StartRecoveringStamina();
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		ResetToWalk();
	}
	else
	{
		
		return;
	}


}

void AZCCharBase::SetGliding()
{
	Debug::Print(TEXT("Gliding"));
	GetCharacterMovement()->AirControl = 0.6f;
	//����Ϊ����ģʽ
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	StartDrainingStamina();
	//����ģ������,ÿִ֡��
	GetWorldTimerManager().SetTimer(AddGravityForFlyingTimerHandle, this,
		&AZCCharBase::AddGravityForFlying, GetWorld()->GetDeltaSeconds(), true);
}

void AZCCharBase::SetWalking()
{
	Debug::Print(TEXT("Walking"));
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	ResetToWalk();
	//�ָ�������
	StartRecoveringStamina();
}

void AZCCharBase::SetFalling()
{
	// ����״̬���ظ�����ֵ���������޻���
	GetCharacterMovement()->AirControl = 0.35f;

	ResetToWalk();
	ClearDrainRecoverTimers();
}

bool const AZCCharBase::IsCharacterExhausted()
{
	bool bEqual = CurrentMT == EMovementTypes::MT_Exhausted;
	return bEqual;
}

#pragma endregion

void AZCCharBase::DrainStaminaTimer()
{
	if (CurStamina <= 0.0f)
	{
		LocomotionManager(EMovementTypes::MT_Exhausted);
	}
	else
	{
		CurStamina = FMath::Clamp((CurStamina - StaminaDepletionAmount), 0.0f, MaxStamina);
	}
}

void AZCCharBase::StartDrainingStamina()
{

	//�������Timer
	ClearDrainRecoverTimers();

	GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, 
		&AZCCharBase::DrainStaminaTimer,StaminaDepletionRate,true);

	//��ʾUI
	if (LayoutRef)
	{
		LayoutRef->ShowGaugeAnim(true);
	}
}

void AZCCharBase::RecoverStaminaTimer()
{
	if (CurStamina < MaxStamina) 
	{
		CurStamina = FMath::Clamp((CurStamina + StaminaDepletionAmount), 0.0f, MaxStamina);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
		LocomotionManager(EMovementTypes::MT_Walking);
		//����UI
		if (LayoutRef)
		{
			LayoutRef->ShowGaugeAnim(false);
		}
	}
}

void AZCCharBase::StartRecoveringStamina()
{
	//�������Timer
	ClearDrainRecoverTimers();

	GetWorldTimerManager().SetTimer(RecoverStaminaTimerHandle, this, 
		&AZCCharBase::RecoverStaminaTimer,StaminaDepletionRate, true);
}

void AZCCharBase::ClearDrainRecoverTimers()
{
	GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
	GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
}

void AZCCharBase::AddGravityForFlying()
{
	LaunchCharacter(FVector(0.0f, 0.0f, -100.0f), false, true);
}
