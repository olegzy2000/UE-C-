// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Camera/CameraComponent.h"
#include "Characters/Controllers/GCPlayerController.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/PlayerComponents/CameraBehaviorComponent.h"
#include "Components/PlayerComponents/OxygenManagerComponent.h"
#include "Components/PlayerComponents/StaminaManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Subsystems/StreamingSubsystem/StreamingSubsystemUtils.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = DefaultSpringArmLength;

	CameraBehaviorComponent = CreateDefaultSubobject<UCameraBehaviorComponent>(TEXT("CameraBehavior"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetBaseCharacterMovementComponent()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	StaminaManagerComponent = CreateDefaultSubobject<UStaminaManagerComponent>(TEXT("StaminaManager"));

	OxygenManagerComponent = CreateDefaultSubobject<UOxygenManagerComponent>(TEXT("OxygenManager"));

	Team = ETeams::Player;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AGCPlayerController>(GetController());

	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->Initialize(SpringArmComponent, CameraComponent);
		CameraBehaviorComponent->InitDefaultBehavior();
	}

	if (StaminaManagerComponent)
	{
		StaminaManagerComponent->OnStaminaDepleted.AddDynamic(this, &APlayerCharacter::OnStaminaDepleted);
		StaminaManagerComponent->OnStaminaRestored.AddDynamic(this, &APlayerCharacter::OnStaminaRestored);
	}
	if (OxygenManagerComponent)
	{
		OxygenManagerComponent->OnOxygenDepleted.AddDynamic(this, &APlayerCharacter::OnOxygenDepleted);
		OxygenManagerComponent->OnOxygenRestored.AddDynamic(this, &APlayerCharacter::OnOxygenRestored);
	}

	bWasSwimmingLastFrame = GetBaseCharacterMovementComponent()->IsSwimming();
	UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(this);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckUnderwaterState(DeltaTime);
}







// ====================  ====================

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	if (!CameraBehaviorComponent) return;

	if (CameraBehaviorComponent->IsAiming())
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon))
		{
			AddControllerYawInput(Value * CurrentRangeWeapon->GetAimTurnModifier());
		}
	}
	else
	{
		AddControllerYawInput(Value);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	if (!CameraBehaviorComponent) return;

	if (CameraBehaviorComponent->IsAiming())
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon))
		{
			AddControllerPitchInput(Value * CurrentRangeWeapon->GetAimLookUpModifier());
		}
	}
	else
	{
		AddControllerPitchInput(Value);
	}
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// ====================  ====================

void APlayerCharacter::SwimForward(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator Rotation = GetControlRotation();
		const FVector ZDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
		AddMovementInput(ZDirection, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

// ====================  ====================

void APlayerCharacter::SwitchCameraPosition()
{
	if (!CameraBehaviorComponent) return;
	CameraBehaviorComponent->SwitchShoulderPosition();
}

// ====================    ====================

void APlayerCharacter::StartAiming()
{
	bIsCallingAimingByFire = false;

	if (GetWorld()->GetTimerManager().IsTimerActive(StopAimTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(StopAimTimerHandle);
	}

	Super::StartAiming();

	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->InitAimBehavior();
	}
}

void APlayerCharacter::StopAiming()
{
	Super::StopAiming();
	bIsCallingAimingByFire = false;

	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->InitDefaultBehavior();
	}
}

void APlayerCharacter::StartFire()
{
	if (!IsAiming())
	{
		StartAiming();
		bIsCallingAimingByFire = true;
	}
	Super::StartFire();
}

void APlayerCharacter::StopFire()
{
	if (bIsCallingAimingByFire && IsAiming())
	{
		GetWorld()->GetTimerManager().SetTimer(StopAimTimerHandle, this, &APlayerCharacter::StopAiming, 2.0f, false);
	}
	Super::StopFire();
}

// ====================  ====================

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

// ====================  ====================

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return (!GetBaseCharacterMovementComponent()->IsProning()) &&
		(bIsCrouched || Super::CanJumpInternal_Implementation()) &&
		(!GetBaseCharacterMovementComponent()->IsMantling()) &&
		(!GetBaseCharacterMovementComponent()->IsRunningOnWall()) &&
		(!GetBaseCharacterMovementComponent()->IsSlide()) &&
		(!GetBaseCharacterMovementComponent()->IsOnZipline()) &&
		(!GetBaseCharacterMovementComponent()->IsOnLadder() &&
			(!GetBaseCharacterMovementComponent()->IsFalling()));
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (GetBaseCharacterMovementComponent()->IsCrouched())
	{
		GetBaseCharacterMovementComponent()->ChangeCrouchState();
	}
}

void APlayerCharacter::Jump()
{
	if (CanJumpInternal_Implementation())
	{
		Super::Jump();
	}
	if (GetBaseCharacterMovementComponent()->IsCrouched())
	{
		Super::ChangeCrouchState();
	}
}

// ====================  ====================

void APlayerCharacter::StartSprint()
{
	Super::StartSprint();
	OnSprintStart_Implementation();
}

void APlayerCharacter::StopSprint()
{
	Super::StopSprint();
	OnSprintEnd_Implementation();
}
void APlayerCharacter::OnSprintStart_Implementation()
{
	if (IsAiming())
	{
		StopAiming();
	}

	if (StaminaManagerComponent && StaminaManagerComponent->CanSprint())
	{
		StaminaManagerComponent->StartStaminaDrain();
	}
	else
	{
		StopSprint();
		return;
	}

	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->StartSprintCameraTransition(SpringArmLengthInSprint);
	}
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	if (StaminaManagerComponent)
	{
		StaminaManagerComponent->StopStaminaDrain();
	}

	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->StopSprintCameraTransition();
	}
}


void APlayerCharacter::Slide()
{
	if (GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->IsSlide())
	{
		StopSprint();
		Super::Slide();
	}
}
// ====================  ====================
void APlayerCharacter::OnStaminaDepleted()
{
	if (GetBaseCharacterMovementComponent())
	{
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = StaminaManagerComponent->GetFatiguedWalkSpeed();
	}

	bCanStartSrpint = false;
	StopSprint();
}

void APlayerCharacter::OnStaminaRestored()
{
	if (GetBaseCharacterMovementComponent() && !GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = StaminaManagerComponent->GetNormalWalkSpeed();
	}

	bCanStartSrpint = true;
}

// ====================  ====================

void APlayerCharacter::ChangeProneState()
{
	Super::ChangeProneState();
}

// ====================    ====================

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}


// ====================  ====================
void APlayerCharacter::CheckUnderwaterState(float DeltaTime)
{
	if (!OxygenManagerComponent || !GetBaseCharacterMovementComponent())
	{
		return;
	}

	const bool bIsSwimming = GetBaseCharacterMovementComponent()->IsSwimming();
	if (bIsSwimming == bWasSwimmingLastFrame)
	{
		return;
	}

	if (bIsSwimming)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Entered water - Starting oxygen drain"));
		OxygenManagerComponent->StartOxygenDrain();
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("Exited water - Starting oxygen restoration"));
		OxygenManagerComponent->StopOxygenDrain();
	}

	bWasSwimmingLastFrame = bIsSwimming;
}

void APlayerCharacter::OnOxygenDepleted()
{
	float DamageAmount = OxygenManagerComponent->GetOxygenDepletionDamage();
	TakeDamage(DamageAmount, FDamageEvent(), GetController(), this);
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OnOxygenDepleted - Applied %f damage"), DamageAmount);
}

void APlayerCharacter::OnOxygenRestored()
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OnOxygenRestored"));
}


// ====================     ====================
void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();

	if (!IsValid(PlayerController) || !CameraBehaviorComponent)
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon))
		{
			float TargetFOV = CurrentRangeWeapon->GetAimFOV();
			CameraBehaviorComponent->StartAimFOVTransition(TargetFOV);
		}
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();

	if (!IsValid(PlayerController) || !CameraBehaviorComponent)
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon))
		{
			CameraBehaviorComponent->StopAimFOVTransition();
		}
	}
}
