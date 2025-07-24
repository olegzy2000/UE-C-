// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"
#include "GameMode/PayerGameModeBaseSecondVersion.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include <string>
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AGCPlayerController>(GetController());

	InitDefaultCameraBehavior();

	InitTimelineToSprintCamera();
	InitStaminaParameters();
	InitHealthParameters();
	InitOxygenParameters();
}
void APlayerCharacter::InitDefaultCameraBehavior()
{
	bUseControllerRotationYaw = false;
	GetBaseCharacterMovementComponent()->bOrientRotationToMovement = true;
}
void APlayerCharacter::InitAimCameraBehavior()
{
	bUseControllerRotationYaw = true;
	GetBaseCharacterMovementComponent()->bOrientRotationToMovement = false;
}
void APlayerCharacter::InitSwimmingCameraBehavior()
{
	
}
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = DefaultSpringArmLenght;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	FVector StartLocation = FVector(0.0f, DefaultPositionOfCamera, 0.0f);
	CameraComponent->SetRelativeLocation(StartLocation);

	GetBaseCharacterMovementComponent()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	InitTimelineCurveToStaminaProgressBar();
	InitTimelineCurveToSprintCamera();
	Team = ETeams::Player;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineForCamera.TickTimeline(DeltaTime);
	TimelineForStaminaProgressBar.TickTimeline(DeltaTime);
	TimelineForAimCamera.TickTimeline(DeltaTime);
	TickOxygen(DeltaTime);
}
void APlayerCharacter::StartAiming() {
	Super::StartAiming();
	InitAimCameraBehavior();
}
void APlayerCharacter::StopAiming() {
	Super::StopAiming();
	InitDefaultCameraBehavior();
}
void APlayerCharacter::MoveForward(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling())&& !FMath::IsNearlyZero(Value, 1e-6f)) {
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}
void APlayerCharacter::MoveRight(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling())  && !FMath::IsNearlyZero(Value, 1e-6f)) {
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}
void APlayerCharacter::Turn(float Value)
{
	if (IsAming()) {
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon)) {
			AddControllerYawInput(Value * CurrentRangeWeapon->GetAimTurnModifier());
		}
	}
	else {
		AddControllerYawInput(Value);
	}
	//AddControllerYawInput(Value);
}
void APlayerCharacter::LookUp(float Value)
{
	if (IsAming()) {
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon)) {
			AddControllerPitchInput(Value*CurrentRangeWeapon->GetAimLookUpModifier());
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
	AddControllerPitchInput(Value*BaseLookUpRate* GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::SwitchCameraPosition()
{
	bIsCameraOnRightPosition = !bIsCameraOnRightPosition;
	if (bIsCameraOnRightPosition) {
		FVector CurrentCameraOffset = FVector(0.f, DefaultPositionOfCamera, 0.f);
		CameraComponent->SetRelativeLocation(CurrentCameraOffset);
	}
	else {
		FVector CurrentCameraOffset = FVector(0.f, -DefaultPositionOfCamera, 0.f);
		CameraComponent->SetRelativeLocation(CurrentCameraOffset);
	}
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f)) {
		FRotator YawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f)) {
		FRotator Rotation = GetControlRotation();
		const FVector ZDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
		AddMovementInput(ZDirection, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f)) {
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

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

void APlayerCharacter::OnSprintEnd_Implementation()
{
	//GetBaseCharacterMovementComponent()->bOrientRotationToMovement = false;
	ReverseResizeSpringArmLength();
	ReverseResizeProgressBarPercent();
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	//GetBaseCharacterMovementComponent()->bOrientRotationToMovement = true;
	StartResizeSpringArmLength();
	StartResizeProgressBarPercent();
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return (!GetBaseCharacterMovementComponent()->IsProning()) && (bIsCrouched || Super::CanJumpInternal_Implementation()) && (!GetBaseCharacterMovementComponent()->IsMantling()) && (!GetBaseCharacterMovementComponent()->IsRunningOnWall()) && (!GetBaseCharacterMovementComponent()->IsSlide()) 
		&& (!GetBaseCharacterMovementComponent()->IsOnZipline()) && (!GetBaseCharacterMovementComponent()->IsOnLadder() && (!GetBaseCharacterMovementComponent()->IsFalling()));
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (GetBaseCharacterMovementComponent()->IsCrouched()) {
		GetBaseCharacterMovementComponent()->ChangeCrouchState();
	}
}

void APlayerCharacter::ChangeProneState()
{
	Super::ChangeProneState();
}
void APlayerCharacter::StartSprint()
{
	Super::StartSprint();
	OnSprintStart_Implementation();
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageAmount=Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	UpdateHealthBar();
	return DamageAmount;
}

void APlayerCharacter::StopSprint()
{
	Super::StopSprint();
	OnSprintEnd_Implementation();
}

void APlayerCharacter::Jump()
{
	if (CanJumpInternal_Implementation()) {
		Super::Jump();
	}
	if (GetBaseCharacterMovementComponent()->IsCrouched()) {
		Super::ChangeCrouchState();
	}
}

void APlayerCharacter::Slide()
{
	if (GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->IsSlide()) {
		StopSprint();
		Super::Slide();
	}
}

void APlayerCharacter::UpdateHealthBar()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetHealthProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetHealthProgressBar()->SetPercent(CharacterAttributesComponent->GetHealth() / 100);
}

void APlayerCharacter::RestoreStaminaProgressBar()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar()) {
		TimelineForStaminaProgressBar.Stop();
		ChangeSpeedParamAfterFatigue();
	   PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxStamina() / 100);
    }
}

void APlayerCharacter::TickOxygen(float DeltaTime)
{
	TimelineForOxygenProgressBar.TickTimeline(DeltaTime);
	if (GetBaseCharacterMovementComponent()->IsSwimming()) {
		FVector HeadPosition = GetMesh()->GetSocketLocation(FName("head"));
		APhysicsVolume* Volume = GetCharacterMovement()->GetPhysicsVolume(); //Получаем теущий физический объем
		float VolumeTopPlane = (Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z); // Берем Z координату для центра данного объема и добавляем к нему половину высота бокса с учетом масштаба 
		if (!IsStartUseOxygen && HeadPosition.Z<VolumeTopPlane) {
			IsStartUseOxygen = true;
			StartProgressBarOxygenPercent();
		}
	}
	else if(IsStartUseOxygen) {
		IsStartUseOxygen = false;
		ReverseProgressBarOxygenPercent();
	}
}



void APlayerCharacter::ChangeSpeedParamAfterFatigue()
{
	if (!GetBaseCharacterMovementComponent()->IsProning()) {
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = 600.0f;
	}
	bCanStartSrpint = true;
	ChangeColorOfProgressBar();
}
void APlayerCharacter::ChangeColorOfProgressBar()
{
	FLinearColor linerColor = FLinearColor(0.066792f, 0.484279f, 1.0f, 1.0f);
	if(IsValid(PlayerController) && PlayerController->GetPlayerHUD()!=nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetFillColorAndOpacity(linerColor);
}

void APlayerCharacter::SpringArmTargetLengthUpdate(float Alpha)
{
	float SpringArmTagretLength = FMath::Lerp(DefaultSpringArmLenght, SpringArmLenghtInSprint, Alpha);
	SpringArmComponent->TargetArmLength = SpringArmTagretLength;
}
void APlayerCharacter::FovToAimUpdate(float Alpha)
{
	float CurrentTickFOV = FMath::Lerp(DefaultFOV, CurrentFOV, Alpha);
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	CameraManager->SetFOV(Alpha);
	//if (CurrentTickFOV >= 90.0f) {
	//	CameraManager->UnlockFOV();
	//}
}
void APlayerCharacter::InitStaminaParameters()
{
	TimeStamina = CharacterAttributesComponent->GetMaxStamina() / CharacterAttributesComponent->GetSpeedDownStamina();
	InitTimelineCurveToStaminaProgressBar();
	InitTimelineToStaminaProgressBar();
	CharacterAttributesComponent->OnRestoreStaminaEvent.AddUObject(this, &APlayerCharacter::RestoreStaminaProgressBar);
	RestoreStaminaProgressBar();
}
void APlayerCharacter::InitHealthParameters()
{
	CharacterAttributesComponent->OnHealthAddEvent.AddUObject(this, &APlayerCharacter::UpdateHealthBar);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetHealthProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetHealthProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxHealth() / 100);
}
void APlayerCharacter::InitOxygenParameters()
{
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetPercent(CharacterAttributesComponent->GetMaxOxygen() / 100);
	InitTimelineCurveToOxygenProgressBar();
	InitTimelineToOxygenProgressBar();
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
void APlayerCharacter::InitTimelineCurveToStaminaProgressBar()
{
	TimeStamina = CharacterAttributesComponent->GetMaxStamina() / CharacterAttributesComponent->GetSpeedDownStamina();
	TimelineCurveForStaminaProgressBar = NewObject<UCurveFloat>();
	FKeyHandle KeyHandleForProgressBar = TimelineCurveForStaminaProgressBar->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForStaminaProgressBar->FloatCurve.AddKey(TimeStamina, CharacterAttributesComponent->GetMaxStamina()/100);
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
	float StaminaPercent = FMath::Lerp(CharacterAttributesComponent->GetMaxStamina()/100, 0.0f, Alpha);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetPercent(StaminaPercent);
	if (StaminaPercent <= 0.0f) {
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = 300.0f;
		if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetStaminaProgressBar()!=nullptr)
			PlayerController->GetPlayerHUD()->GetStaminaProgressBar()->SetFillColorAndOpacity(FLinearColor::Red);
		bCanStartSrpint = false;
		StopSprint();
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &APlayerCharacter::ChangeSpeedParamAfterFatigue, TimeStamina, false);
	}
	if (StaminaPercent == 0.0f || !GetBaseCharacterMovementComponent()->IsSprinting()) {
		StopSprint();
	}
}

void APlayerCharacter::OxygenProgressBarUpdate(float alpha)
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OxygenProgressBarUpdate"));
	float OxygenPercent = FMath::Lerp(CharacterAttributesComponent->GetMaxOxygen() / 100, 0.0f, alpha);
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetPercent(OxygenPercent);
	if (OxygenPercent <= 0.0f) {
		TakeDamage(CharacterAttributesComponent->GetMaxHealth(), FDamageEvent(), GetController(), this);
		UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::OxygenProgressBarUpdate FINISH"));
	}
}

void APlayerCharacter::StartResizeSpringArmLength()
{
	if (TimelineForCamera.IsReversing()) {
		TimelineForCamera.Stop();
	}
	TimelineForCamera.Play();
}

void APlayerCharacter::ReverseResizeSpringArmLength()
{
	if (TimelineForCamera.IsPlaying()) {
		TimelineForCamera.Stop();
	}
	TimelineForCamera.Reverse();
}

void APlayerCharacter::StartResizeProgressBarPercent()
{
	if (TimelineForStaminaProgressBar.IsReversing()) {
		TimelineForStaminaProgressBar.Stop();
	}
	TimelineForStaminaProgressBar.Play();
}

void APlayerCharacter::ReverseResizeProgressBarPercent()
{
	if (TimelineForStaminaProgressBar.IsPlaying()) {
		TimelineForStaminaProgressBar.Stop();
	}
	TimelineForStaminaProgressBar.Reverse();
}

void APlayerCharacter::StartProgressBarOxygenPercent()
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::StartProgressBarOxygenPercent()"));
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar()!=nullptr)
	PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetVisibility(ESlateVisibility::Visible);
	if (TimelineForOxygenProgressBar.IsReversing()) {
		TimelineForOxygenProgressBar.Stop();
	}
	TimelineForOxygenProgressBar.Play();
}

void APlayerCharacter::ReverseProgressBarOxygenPercent()
{
	UE_LOG(LogDamage, Warning, TEXT("APlayerCharacter::ReverseProgressBarOxygenPercent()"));
	if (IsValid(PlayerController) && PlayerController->GetPlayerHUD() != nullptr && PlayerController->GetPlayerHUD()->GetOxygenProgressBar() != nullptr)
		PlayerController->GetPlayerHUD()->GetOxygenProgressBar()->SetVisibility(ESlateVisibility::Visible);
	if (TimelineForOxygenProgressBar.IsPlaying()) {
		TimelineForOxygenProgressBar.Stop();
	}
	TimelineForOxygenProgressBar.Reverse();
}

void APlayerCharacter::InitTimelineCurveForAimCamera()
{
	TimelineCurveForAimCamera = NewObject<UCurveFloat>();;
	FKeyHandle KeyHandleForCamera = TimelineCurveForAimCamera->FloatCurve.AddKey(0.f, 90.f);
	TimelineCurveForAimCamera->FloatCurve.AddKey(TimeToAim, CurrentFOV);
	TimelineCurveForAimCamera->FloatCurve.SetKeyInterpMode(KeyHandleForCamera, ERichCurveInterpMode::RCIM_Linear, true);
}
void APlayerCharacter::InitTimelineToSprintCamera()
{
	FOnTimelineFloatStatic SpringArmMovementTimeLineUpdate;
	SpringArmMovementTimeLineUpdate.BindUObject(this, &APlayerCharacter::SpringArmTargetLengthUpdate);
	TimelineForCamera.AddInterpFloat(TimelineCurveForCamera, SpringArmMovementTimeLineUpdate);
	TimelineForCamera.SetTimelineLength(TimeToSwitchPositionCameraInSprint);
	TimelineForCamera.SetLooping(false);
}
void APlayerCharacter::InitTimelineCurveToSprintCamera()
{
	TimelineCurveForCamera = CreateDefaultSubobject<UCurveFloat>(TEXT("Timeline Curve for camera"));
	FKeyHandle KeyHandleForCamera = TimelineCurveForCamera->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForCamera->FloatCurve.AddKey(TimeStamina, 100.0f);
	TimelineCurveForCamera->FloatCurve.SetKeyInterpMode(KeyHandleForCamera, ERichCurveInterpMode::RCIM_Linear, true);
}
void APlayerCharacter::InitTimelineToAimCamera()
{
	InitTimelineCurveForAimCamera();
	FOnTimelineFloatStatic FOVMovementTimeLineUpdate;
	FOVMovementTimeLineUpdate.BindUObject(this, &APlayerCharacter::FovToAimUpdate);
	TimelineForAimCamera.AddInterpFloat(TimelineCurveForAimCamera, FOVMovementTimeLineUpdate);
	TimelineForAimCamera.SetTimelineLength(TimeToAim);
	TimelineForAimCamera.SetLooping(false);
}
void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	if (!IsValid(PlayerController)) {
		return;
	}
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager)) {
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon)) {
			DefaultFOV = 90.0f;
			CurrentFOV = CurrentRangeWeapon->GetAimFOV();//50.0f;
			InitTimelineToAimCamera();
			TimelineForAimCamera.Play();
		}
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	if (!IsValid(PlayerController)) {
		return;
	}
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager)) {
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon)) {
			TimelineForAimCamera.Reverse();
		}
	}
}
