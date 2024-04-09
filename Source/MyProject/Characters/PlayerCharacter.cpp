// Fill out your copyright notice in the Description page of Project Settings.

#include "../GameMode/PayerGameModeBaseSecondVersion.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<APayerGameModeBaseSecondVersion>(UGameplayStatics::GetGameMode(GetWorld()));
	InitTimelineToSprintCamera();
	InitTimelineToFatigureProgressBar();
	GameMode->GetCurrentWidget()->GetFatigueBar()->SetPercent(MaxFatigue);
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
	AddControllerYawInput(Value);
}
void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value*BaseTurnRate*GetWorld()->GetDeltaSeconds());
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
	ReverseResizeSpringArmLength();
	ReverseResizeProgressBarPercent();
}

void APlayerCharacter::OnSprintStart_Implementation()
{
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

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineForCamera.TickTimeline(DeltaTime);
	TimelineForFatigueProgressBar.TickTimeline(DeltaTime);
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
	SpringArmComponent->TargetArmLength= DefaultSpringArmLenght;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
	FVector StartLocation = FVector(0.0f, DefaultPositionOfCamera, 0.0f);
	CameraComponent->SetRelativeLocation(StartLocation);
	GetBaseCharacterMovementComponent()->bOrientRotationToMovement=1;
	GetBaseCharacterMovementComponent()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	InitTimelineCurveToFatigureProgressBar();
	InitTimelineCurveToSprintCamera();
}
void APlayerCharacter::InitTimelineToSprintCamera()
{
	FOnTimelineFloatStatic SpringArmMovementTimeLineUpdate;
	SpringArmMovementTimeLineUpdate.BindUObject(this, &APlayerCharacter::SpringArmTargetLengthUpdate);
	TimelineForCamera.AddInterpFloat(TimelineCurveForCamera, SpringArmMovementTimeLineUpdate);
	TimelineForCamera.SetTimelineLength(TimeToSwitchPositionCameraInSprint);
	TimelineForCamera.SetLooping(false);
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
	GameMode->GetCurrentWidget()->GetFatigueBar()->SetFillColorAndOpacity(linerColor);
}
void APlayerCharacter::InitTimelineToFatigureProgressBar()
{
	FOnTimelineFloatStatic ProgressBarTimeLineUpdate;
	ProgressBarTimeLineUpdate.BindUObject(this, &APlayerCharacter::FatigueProgressBarUpdate);
	TimelineForFatigueProgressBar.AddInterpFloat(TimelineCurveForProgressBar, ProgressBarTimeLineUpdate);
	TimelineForFatigueProgressBar.SetTimelineLength(TimeFatigue);
	TimelineForFatigueProgressBar.SetLooping(false);
}
void APlayerCharacter::InitTimelineCurveToSprintCamera()
{
	TimelineCurveForCamera = CreateDefaultSubobject<UCurveFloat>(TEXT("Timeline Curve for camera"));
	FKeyHandle KeyHandleForCamera = TimelineCurveForCamera->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForCamera->FloatCurve.AddKey(0.5f, 1.0f);
	TimelineCurveForCamera->FloatCurve.SetKeyInterpMode(KeyHandleForCamera, ERichCurveInterpMode::RCIM_Linear, true);
}
void APlayerCharacter::InitTimelineCurveToFatigureProgressBar()
{
	TimelineCurveForProgressBar = CreateDefaultSubobject<UCurveFloat>(TEXT("Timeline Curve for progress bar"));
	FKeyHandle KeyHandleForProgressBar = TimelineCurveForProgressBar->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurveForProgressBar->FloatCurve.AddKey(TimeFatigue, 1.0f);
	TimelineCurveForProgressBar->FloatCurve.SetKeyInterpMode(KeyHandleForProgressBar, ERichCurveInterpMode::RCIM_Linear, true);
}
void APlayerCharacter::SpringArmTargetLengthUpdate(float Alpha)
{
		float SpringArmTagretLength = FMath::Lerp(DefaultSpringArmLenght, SpringArmLenghtInSprint, Alpha);
		SpringArmComponent->TargetArmLength = SpringArmTagretLength;
}
void APlayerCharacter::FatigueProgressBarUpdate(float Alpha)
{
	float FatiguePercent = FMath::Lerp(MaxFatigue, MinFatigue, Alpha*SpeedFatigue);
	GameMode->GetCurrentWidget()->GetFatigueBar()->SetPercent(FatiguePercent);
	if (FatiguePercent == MinFatigue) {
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = 300.0f;
		GameMode->GetCurrentWidget()->GetFatigueBar()->SetFillColorAndOpacity(FLinearColor::Red);
		bCanStartSrpint = false;
		StopSprint();
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &APlayerCharacter::ChangeSpeedParamAfterFatigue, TimeFatigue, false);
	}
	if (FatiguePercent == MinFatigue || !GetBaseCharacterMovementComponent()->IsSprinting()) {
		StopSprint();
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
	if (TimelineForFatigueProgressBar.IsReversing()) {
		TimelineForFatigueProgressBar.Stop();
	}
	TimelineForFatigueProgressBar.Play();
}

void APlayerCharacter::ReverseResizeProgressBarPercent()
{
	if (TimelineForFatigueProgressBar.IsPlaying()) {
		TimelineForFatigueProgressBar.Stop();
	}
	TimelineForFatigueProgressBar.Reverse();
}
