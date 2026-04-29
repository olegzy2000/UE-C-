// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "GameMode/PayerGameModeBaseSecondVersion.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include <string>
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Subsystems/StreamingSubsystem/StreamingSubsystemUtils.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Настройка вращения
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Настройка SpringArm
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = DefaultSpringArmLength;

	// Настройка камеры
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	FVector StartLocation = FVector(0.0f, DefaultPositionOfCamera, 0.0f);
	CameraComponent->SetRelativeLocation(StartLocation);

	// Настройка движения
	GetBaseCharacterMovementComponent()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	// Создание компонента поведения камеры
	CameraBehaviorComponent = CreateDefaultSubobject<UCameraBehaviorComponent>(TEXT("CameraBehavior"));

	// Инициализация кривых
	InitTimelineCurveToStaminaProgressBar();
	InitTimelineCurveToOxygenProgressBar();

	// Команда
	Team = ETeams::Player;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Кешируем контроллер
	PlayerController = Cast<AGCPlayerController>(GetController());

	// Инициализируем компонент камеры
	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->Initialize(SpringArmComponent, CameraComponent);
		CameraBehaviorComponent->InitDefaultBehavior();
	}

	// Инициализация параметров
	InitStaminaParameters();
	InitHealthParameters();
	InitOxygenParameters();

	// Проверка StreamingSubsystem
	UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(this);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Тики кислорода и стамины (временно, пока не вынесено)
	TickOxygen(DeltaTime);
	TimelineForStaminaProgressBar.TickTimeline(DeltaTime);
}

// ==================== ДВИЖЕНИЕ ====================

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

// ==================== ПЛАВАНИЕ ====================

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

// ==================== КАМЕРА ====================

void APlayerCharacter::SwitchCameraPosition()
{
	if (!CameraBehaviorComponent) return;

	bIsCameraOnRightPosition = !bIsCameraOnRightPosition;
	CameraBehaviorComponent->SwitchShoulderPosition(DefaultPositionOfCamera, bIsCameraOnRightPosition);
}

// ==================== ПРИЦЕЛИВАНИЕ И СТРЕЛЬБА ====================

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
	if (!IsAming())
	{
		StartAiming();
		bIsCallingAimingByFire = true;
	}
	Super::StartFire();
}

void APlayerCharacter::StopFire()
{
	if (bIsCallingAimingByFire && IsAming())
	{
		GetWorld()->GetTimerManager().SetTimer(StopAimTimerHandle, this, &APlayerCharacter::StopAiming, 2.0f, false);
	}
	Super::StopFire();
}

// ==================== ПРИСЕДАНИЕ ====================

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

// ==================== ПРЫЖКИ ====================

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

// ==================== СПРИНТ ====================

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
	StartResizeProgressBarPercent();
	if (CameraBehaviorComponent)
	{
		CameraBehaviorComponent->StartSprintCameraTransition(SpringArmLengthInSprint);
	}
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	ReverseResizeProgressBarPercent();
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

// ==================== ЛЕЖАНИЕ ====================

void APlayerCharacter::ChangeProneState()
{
	Super::ChangeProneState();
}

// ==================== УРОН И ЗДОРОВЬЕ ====================

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	UpdateHealthBar();
	return DamageAmount;
}

void APlayerCharacter::UpdateHealthBar()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetHealthProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetHealthProgressBar()->SetPercent(CharacterAttributesComponent->GetHealth() / 100);
	}
}

// ==================== СТАМИНА ====================

void APlayerCharacter::InitStaminaParameters()
{
	TimeStamina = CharacterAttributesComponent->GetMaxStamina() / CharacterAttributesComponent->GetSpeedDownStamina();
	InitTimelineCurveToStaminaProgressBar();
	InitTimelineToStaminaProgressBar();
	CharacterAttributesComponent->OnRestoreStaminaEvent.AddUObject(this, &APlayerCharacter::RestoreStaminaProgressBar);
	RestoreStaminaProgressBar();
}

void APlayerCharacter::RestoreStaminaProgressBar()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar())
	{
		TimelineForStaminaProgressBar.Stop();
		ChangeSpeedParamAfterFatigue();
		PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxStamina() / 100);
	}
}

void APlayerCharacter::ChangeSpeedParamAfterFatigue()
{
	if (!GetBaseCharacterMovementComponent()->IsProning())
	{
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = 600.0f;
	}
	bCanStartSrpint = true;
	ChangeColorOfProgressBar();
}

void APlayerCharacter::ChangeColorOfProgressBar()
{
	FLinearColor linerColor = FLinearColor(0.066792f, 0.484279f, 1.0f, 1.0f);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetFillColorAndOpacity(linerColor);
	}
}

void APlayerCharacter::InitTimelineCurveToStaminaProgressBar()
{
	TimeStamina = CharacterAttributesComponent->GetMaxStamina() / CharacterAttributesComponent->GetSpeedDownStamina();
	TimelineCurveForStaminaProgressBar = NewObject<UCurveFloat>();
	FKeyHandle KeyHandleForProgressBar = TimelineCurveForStaminaProgressBar->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForStaminaProgressBar->FloatCurve.AddKey(TimeStamina, CharacterAttributesComponent->GetMaxStamina() / 100);
	TimelineCurveForStaminaProgressBar->FloatCurve.SetKeyInterpMode(KeyHandleForProgressBar, ERichCurveInterpMode::RCIM_Linear, true);
}

void APlayerCharacter::InitTimelineToStaminaProgressBar()
{
	FOnTimelineFloatStatic ProgressBarTimeLineUpdate;
	ProgressBarTimeLineUpdate.BindUObject(this, &APlayerCharacter::StaminaProgressBarUpdate);
	TimelineForStaminaProgressBar.AddInterpFloat(TimelineCurveForStaminaProgressBar, ProgressBarTimeLineUpdate);
	TimelineForStaminaProgressBar.SetTimelineLength(TimeStamina);
	TimelineForStaminaProgressBar.SetLooping(false);
}

void APlayerCharacter::StaminaProgressBarUpdate(float Alpha)
{
	float StaminaPercent = FMath::Lerp(CharacterAttributesComponent->GetMaxStamina() / 100, 0.0f, Alpha);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetPercent(StaminaPercent);
	}

	if (StaminaPercent <= 0.0f)
	{
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = 300.0f;
		if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar() != nullptr)
		{
			PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetFillColorAndOpacity(FLinearColor::Red);
		}
		bCanStartSrpint = false;
		StopSprint();
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &APlayerCharacter::ChangeSpeedParamAfterFatigue, TimeStamina, false);
	}

	if (StaminaPercent == 0.0f || !GetBaseCharacterMovementComponent()->IsSprinting())
	{
		StopSprint();
	}
}

void APlayerCharacter::StartResizeProgressBarPercent()
{
	if (TimelineForStaminaProgressBar.IsReversing())
	{
		TimelineForStaminaProgressBar.Stop();
	}
	TimelineForStaminaProgressBar.Play();
}

void APlayerCharacter::ReverseResizeProgressBarPercent()
{
	if (TimelineForStaminaProgressBar.IsPlaying())
	{
		TimelineForStaminaProgressBar.Stop();
	}
	TimelineForStaminaProgressBar.Reverse();
}

// ==================== КИСЛОРОД ====================

void APlayerCharacter::InitHealthParameters()
{
	CharacterAttributesComponent->OnHealthAddEvent.AddUObject(this, &APlayerCharacter::UpdateHealthBar);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetHealthProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetHealthProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxHealth() / 100);
	}
}

void APlayerCharacter::InitOxygenParameters()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxOxygen() / 100);
	}
	InitTimelineCurveToOxygenProgressBar();
	InitTimelineToOxygenProgressBar();
}

void APlayerCharacter::TickOxygen(float DeltaTime)
{
	TimelineForOxygenProgressBar.TickTimeline(DeltaTime);

	if (GetBaseCharacterMovementComponent()->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetSocketLocation(FName("head"));
		APhysicsVolume* Volume = GetCharacterMovement()->GetPhysicsVolume();
		float VolumeTopPlane = (Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z);

		if (!IsStartUseOxygen && HeadPosition.Z < VolumeTopPlane)
		{
			IsStartUseOxygen = true;
			StartProgressBarOxygenPercent();
		}
	}
	else if (IsStartUseOxygen)
	{
		IsStartUseOxygen = false;
		ReverseProgressBarOxygenPercent();
	}
}

void APlayerCharacter::InitTimelineToOxygenProgressBar()
{
	FOnTimelineFloatStatic ProgressBarTimeLineUpdate;
	ProgressBarTimeLineUpdate.BindUObject(this, &APlayerCharacter::OxygenProgressBarUpdate);
	TimelineForOxygenProgressBar.AddInterpFloat(TimelineCurveForOxygenProgressBar, ProgressBarTimeLineUpdate);
	TimelineForOxygenProgressBar.SetTimelineLength(TimeOxygen);
	TimelineForOxygenProgressBar.SetLooping(false);
}

void APlayerCharacter::InitTimelineCurveToOxygenProgressBar()
{
	TimeOxygen = CharacterAttributesComponent->GetMaxOxygen() / CharacterAttributesComponent->GetOxygenRestoreVelocity();
	TimelineCurveForOxygenProgressBar = NewObject<UCurveFloat>();
	FKeyHandle KeyHandleForProgressBar = TimelineCurveForOxygenProgressBar->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForOxygenProgressBar->FloatCurve.AddKey(TimeOxygen, CharacterAttributesComponent->GetMaxOxygen() / 100);
	TimelineCurveForOxygenProgressBar->FloatCurve.SetKeyInterpMode(KeyHandleForProgressBar, ERichCurveInterpMode::RCIM_Linear, true);
}

void APlayerCharacter::OxygenProgressBarUpdate(float alpha)
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OxygenProgressBarUpdate"));
	float OxygenPercent = FMath::Lerp(CharacterAttributesComponent->GetMaxOxygen() / 100, 0.0f, alpha);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetPercent(OxygenPercent);
	}

	if (OxygenPercent <= 0.0f)
	{
		TakeDamage(CharacterAttributesComponent->GetMaxHealth(), FDamageEvent(), GetController(), this);
		UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OxygenProgressBarUpdate FINISH"));
	}
}

void APlayerCharacter::StartProgressBarOxygenPercent()
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::StartProgressBarOxygenPercent()"));
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetVisibility(ESlateVisibility::Visible);
	}

	if (TimelineForOxygenProgressBar.IsReversing())
	{
		TimelineForOxygenProgressBar.Stop();
	}
	TimelineForOxygenProgressBar.Play();
}

void APlayerCharacter::ReverseProgressBarOxygenPercent()
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::ReverseProgressBarOxygenPercent()"));
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar() != nullptr)
	{
		PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetVisibility(ESlateVisibility::Visible);
	}

	if (TimelineForOxygenProgressBar.IsPlaying())
	{
		TimelineForOxygenProgressBar.Stop();
	}
	TimelineForOxygenProgressBar.Reverse();
}

// ==================== ВНУТРЕННИЕ КОЛБЭКИ ДЛЯ КАМЕРЫ ====================
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
			float TargetFOV = CurrentRangeWeapon->GetAimFOV(); // Например, 50.0f
			// Начинаем интерполяцию FOV к прицельному значению
			CameraBehaviorComponent->StartAimFOVTransition(TargetFOV, TimeToAim);
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
			float TimeToExitAim = 0.3; // Можно добавить в оружие, или использовать 0.3f

			// Плавно возвращаем FOV к исходному значению
			CameraBehaviorComponent->StopAimFOVTransition(TimeToExitAim);
		}
	}
}