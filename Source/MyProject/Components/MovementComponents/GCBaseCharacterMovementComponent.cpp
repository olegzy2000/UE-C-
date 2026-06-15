// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacterMovementComponent.h"
#include "../../Characters/GCBaseCharacter.h"
#include "../../Utils/GCTraceUtils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogLadderAttach, Log, All);

UGCBaseCharacterMovementComponent::UGCBaseCharacterMovementComponent()
{
	RunWallDetectorComponent = CreateDefaultSubobject<URunWallDetectorComponent>(TEXT("RunWallDetectorComponent"));
}

bool UGCBaseCharacterMovementComponent::IsProning()
{
	return bIsProning;
}
bool UGCBaseCharacterMovementComponent::IsCrouched()
{
	return bIsCrouched;
}

void UGCBaseCharacterMovementComponent::SetLadderInput(float Value)
{
	if (IsMovementBlockedBy(EMovementBlockReason::LadderAttach))
	{
		LadderInput = 0.0f;
		return;
	}

	LadderInput = FMath::Clamp(Value, -1.0f, 1.0f);
}
bool UGCBaseCharacterMovementComponent::IsSprinting()
{
	return bIsSprinting;
}
void UGCBaseCharacterMovementComponent::SetIsSprinting(bool flag)
{
	bIsSprinting = flag;
}

void UGCBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bIsSprinting = (Flags &= FSavedMove_Character::FLAG_Custom_0) != 0;
}

FNetworkPredictionData_Client* UGCBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr) {
		UGCBaseCharacterMovementComponent* MutableThis = const_cast<UGCBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_GC(*this);
	}
	return ClientPredictionData;
}

bool UGCBaseCharacterMovementComponent::IsSlide()
{
	return UpdatedComponent && MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Slide;
}
float UGCBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting) {
		Result = SprintSpeed;
	}
	else if (IsOnLadder()) {
		Result = ClimbingOnLadderMaxSpeed;
	}
	else if (GetBaseCharacterOwner()->IsAiming()) {
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
	return Result;
}

bool UGCBaseCharacterMovementComponent::GetIsLeft()
{
	return RunWallDescription.bIsLeft;
}

void UGCBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UGCBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UGCBaseCharacterMovementComponent::ChangeProneState()
{

	bIsProning = !bIsProning;
}

void UGCBaseCharacterMovementComponent::ChangeCrouchState()
{
	bIsCrouched = !bIsCrouched;
}

void UGCBaseCharacterMovementComponent::TryToRunWall()
{
	if (IsFalling() && !IsRunningOnWall()) {
		RunWallDescription = RunWallDetectorComponent->DetectionWall();
		if (RunWallDescription.CanRun)
			StartRunOnWall();
	}
	else if (IsRunningOnWall()) {
		EndRunningOnWall(EDetachFromRunWallMethod::JumpOff);
	}
}

void UGCBaseCharacterMovementComponent::TryToSlide()
{
	if (IsSlide()) {
		SlideStop();
	}
	else {
		SlideStart();
	}

}

void UGCBaseCharacterMovementComponent::SlideStart()
{

	GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius(), SlideCaspsuleHalfHeight);
	GetCharacterOwner()->GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, (SlideCaspsuleHalfHeight - 88)), GetCharacterOwner()->GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	GetCharacterOwner()->GetMesh()->MoveComponent(FVector(0.f, 0.f, -(SlideCaspsuleHalfHeight - 88) - 10), GetCharacterOwner()->GetMesh()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	FTimerHandle TimerHandleSlideStop;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleSlideStop, this, &UGCBaseCharacterMovementComponent::SlideStop, SlideMaxTime, false);
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Slide);
}

void UGCBaseCharacterMovementComponent::SlideStop()
{
	if (IsSlide()) {
		GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius(), 88.0f);
		GetCharacterOwner()->GetMesh()->MoveComponent(FVector(0.f, 0.f, (SlideCaspsuleHalfHeight - 88) + 10), GetCharacterOwner()->GetMesh()->GetComponentQuat()
			, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		SetMovementMode(MOVE_Walking);
	}
}

void UGCBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UGCBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

void UGCBaseCharacterMovementComponent::StartRunOnWall()
{
	GetOwner()->SetActorRotation(RunWallDescription.Rotation);
	SetPlaneConstraintNormal(FVector(0, 0, 1));
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &UGCBaseCharacterMovementComponent::EndRunningOnWall, EDetachFromRunWallMethod::Fall);
	GetWorld()->GetTimerManager().SetTimer(RunWallTimer, RespawnDelegate, 1.0f, false);
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_RunWall);
}

bool UGCBaseCharacterMovementComponent::IsMantling()
{
	return UpdatedComponent && MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

bool UGCBaseCharacterMovementComponent::IsRunningOnWall()
{
	return UpdatedComponent && MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_RunWall;
}


FRotator UGCBaseCharacterMovementComponent::GetLadderTargetRotation() const
{
	if (!IsValid(CurrentLadder))
	{
		return FRotator::ZeroRotator;
	}

	FRotator TargetOrientationRotation =
		CurrentLadder->GetActorForwardVector().ToOrientationRotator();

	TargetOrientationRotation.Yaw += 180.0f;
	TargetOrientationRotation.Pitch = 0.0f;
	TargetOrientationRotation.Roll = 0.0f;

	return TargetOrientationRotation;
}

FVector UGCBaseCharacterMovementComponent::GetLadderAlignedLocation(float LadderProjection) const
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::GetLadderAlignedLocation() cannot be invoked when ladder is null"));

	return CurrentLadder->GetActorLocation()
		+ LadderProjection * CurrentLadder->GetActorUpVector()
		+ LadderToCharacterOffset * CurrentLadder->GetActorForwardVector();
}

void UGCBaseCharacterMovementComponent::ResetLadderAttach()
{
	LadderAttachState.Reset();
}

void UGCBaseCharacterMovementComponent::StartLadderAttachFromTop()
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::StartLadderAttachFromTop() cannot be invoked when ladder is null"));

	BlockMovement(EMovementBlockReason::LadderAttach);

	StopMovementImmediately();
	StopActiveMovement();
	ClearAccumulatedForces();

	LadderAttachState.StartLocation = UpdatedComponent
		? UpdatedComponent->GetComponentLocation()
		: GetOwner()->GetActorLocation();

	LadderAttachState.StartRotation = UpdatedComponent
		? UpdatedComponent->GetComponentRotation()
		: GetOwner()->GetActorRotation();

	const float TargetProjection =
		CurrentLadder->GetLadderHeight() - LadderAttachSettings.TopAttachStartOffset;

	LadderAttachState.TargetLocation = GetLadderAlignedLocation(TargetProjection);
	LadderAttachState.TargetRotation = GetLadderTargetRotation();
	LadderAttachState.bInProgress = true;
	LadderAttachState.bHorizontalMoveCompleted = false;

	UE_LOG(LogLadderAttach, Warning, TEXT(
		"StartLadderAttachFromTop | Character=%s | Start=%s | Target=%s | StartProjection=%.2f | TargetProjection=%.2f | LadderHeight=%.2f | TopAttachStartOffset=%.2f | HorizontalSpeed=%.2f | VerticalSpeed=%.2f"
	),
		*GetNameSafe(GetOwner()),
		*LadderAttachState.StartLocation.ToString(),
		*LadderAttachState.TargetLocation.ToString(),
		GetActorToCurrentLadderProjection(LadderAttachState.StartLocation),
		GetActorToCurrentLadderProjection(LadderAttachState.TargetLocation),
		CurrentLadder->GetLadderHeight(),
		LadderAttachSettings.TopAttachStartOffset,
		LadderAttachSettings.HorizontalSpeed,
		LadderAttachSettings.VerticalSpeed
	);
}

bool UGCBaseCharacterMovementComponent::IsLadderAttachInProgress() const
{
	return LadderAttachState.bInProgress;
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;

	if (!IsValid(CurrentLadder))
	{
		return;
	}

	const bool bAttachFromTop = CurrentLadder->GetIsOnTop();

	if (bAttachFromTop)
	{
		StartLadderAttachFromTop();

		GetOwner()->SetActorLocation(LadderAttachState.StartLocation);
		GetOwner()->SetActorRotation(LadderAttachState.StartRotation);
	}
	else
	{
		ResetLadderAttach();
		UnblockMovement(EMovementBlockReason::LadderAttach);

		const float ActorToLadderProjection =
			GetActorToCurrentLadderProjection(GetOwner()->GetActorLocation());

		const FVector NewCharacterLocation =
			GetLadderAlignedLocation(ActorToLadderProjection);

		const FRotator TargetOrientationRotation =
			GetLadderTargetRotation();

		GetOwner()->SetActorLocation(NewCharacterLocation);
		GetOwner()->SetActorRotation(TargetOrientationRotation);

		UE_LOG(LogLadderAttach, Warning, TEXT(
			"AttachToLadder BOTTOM | Character=%s | Location=%s | Projection=%.2f"
		),
			*GetNameSafe(GetOwner()),
			*NewCharacterLocation.ToString(),
			GetActorToCurrentLadderProjection(NewCharacterLocation)
		);
	}

	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	LadderInput = 0.0f;

	ConsumeInputVector();

	if (CharacterOwner)
	{
		CharacterOwner->ConsumeMovementInputVector();
	}

	SetMovementMode(
		EMovementMode::MOVE_Custom,
		(uint8)ECustomMovementMode::CMOVE_Ladder
	);
}
void UGCBaseCharacterMovementComponent::AttachToZipline(AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FRotator TargetOrientationRotation = CurrentZipline->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;
	FVector StartLocation;
	FVector EndLocation;
	FQuat WorldSocketRotation = FQuat();
	CurrentZipline->GetLeftRailMeshComponent()->GetSocketWorldLocationAndRotation(FName("AttachPoint"), StartLocation, WorldSocketRotation);
	CurrentZipline->GetRightRailMeshComponent()->GetSocketWorldLocationAndRotation(FName("AttachPoint"), EndLocation, WorldSocketRotation);
	FVector ToEndPoint = EndLocation - GetBaseCharacterOwner()->GetActorLocation();
	FVector ToStartPoint = StartLocation - GetBaseCharacterOwner()->GetActorLocation();
	FVector FromStartToEnd = EndLocation - StartLocation;
	FVector StartCharacterLocation = StartLocation - UKismetMathLibrary::ProjectVectorOnToVector(ToStartPoint, FromStartToEnd);
	GetBaseCharacterOwner()->SetActorLocation(FVector(StartCharacterLocation.X, StartCharacterLocation.Y, StartCharacterLocation.Z - ZiplineOffset));
	if (((acosf(FVector::DotProduct(GetBaseCharacterOwner()->GetActorRotation().Vector(), ToStartPoint.ToOrientationRotator().Vector()))) * (180 / 3.1415926)) < ((acosf(FVector::DotProduct(GetBaseCharacterOwner()->GetActorRotation().Vector(), ToEndPoint.ToOrientationRotator().Vector()))) * (180 / 3.1415926))) {
		CurrentZipline->SetEndLocationMove(StartLocation - FVector(0, 0, ZiplineOffset));
	}
	else {
		CurrentZipline->SetEndLocationMove(EndLocation - FVector(0, 0, ZiplineOffset));
	}
	GetBaseCharacterOwner()->SetActorRotation((CurrentZipline->GetEndLocationMove() - GetBaseCharacterOwner()->GetActorLocation()).ToOrientationRotator());
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);

}
float UGCBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location)
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() cannot be invoke when ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	float ActorToLadderProjection = FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
	return ActorToLadderProjection;
}
void UGCBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::ReachingTheBottom: {
		SetMovementMode(EMovementMode::MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop: {
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::JumpOff: {
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(EMovementMode::MOVE_Falling);
		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::Fall:
	default: {
		SetMovementMode(EMovementMode::MOVE_Falling);
		break;
	}
		   break;
	}

}
void UGCBaseCharacterMovementComponent::DetachFromZipline(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::JumpOff: {
		FVector JumpDirection = CharacterOwner->GetActorRotation().Vector();
		SetMovementMode(EMovementMode::MOVE_Falling);
		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::Fall:
	default: {
		SetMovementMode(EMovementMode::MOVE_Falling);
		break;
	}
		   break;
	}
	FRotator finalRotator = CharacterOwner->GetActorRotation();
	CharacterOwner->SetActorRotation(FRotator(0.0f, finalRotator.Yaw, finalRotator.Roll));
	bForceRotation = false;
}
void UGCBaseCharacterMovementComponent::EndRunningOnWall(EDetachFromRunWallMethod DetachType = EDetachFromRunWallMethod::Fall)
{
	if (IsRunningOnWall()) {
		switch (DetachType)
		{
		case EDetachFromRunWallMethod::Fall: {
			SetMovementMode(EMovementMode::MOVE_Falling);
			break;
		}
		case EDetachFromRunWallMethod::JumpOff: {
			FVector JumpDirection;
			if (RunWallDescription.bIsLeft)
				JumpDirection = GetOwner()->GetActorRightVector() + GetOwner()->GetActorForwardVector();
			else
				JumpDirection = -GetOwner()->GetActorRightVector() + GetOwner()->GetActorForwardVector();
			SetMovementMode(EMovementMode::MOVE_Falling);
			FVector JumpVelocity = (JumpDirection)*JumpOffFromRunWall;
			JumpVelocity += FVector(0, 0, 250);
			ForceTargetRotation = JumpDirection.ToOrientationRotator();
			bForceRotation = true;
			Launch(JumpVelocity);
			break;
		}
		default: {
			SetMovementMode(EMovementMode::MOVE_Falling);
			break;
		}
			   break;
		}
	}

}

bool UGCBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}
bool UGCBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

const ALadder* UGCBaseCharacterMovementComponent::GetCurrentLadder()
{
	return CurrentLadder;
}

const AZipline* UGCBaseCharacterMovementComponent::GetCurrentZipline()
{
	return CurrentZipline;
}

void UGCBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation) {
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));
		const float AngleTolerance = 1e-3f;
		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, ForceTargetRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, false);
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}
	else if (!IsOnLadder())
		Super::PhysicsRotation(DeltaTime);
}

float UGCBaseCharacterMovementComponent::GetLadderSpeedRation() const
{
	checkf(IsValid(CurrentLadder), TEXT("UGCBaseCharacterMovementComponent::GetLadderSpeedRation() cannot be invoke when ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

AGCBaseCharacter* UGCBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return Cast<AGCBaseCharacter>(CharacterOwner);
}

void UGCBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviusMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviusMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming) {
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfSize);
	}
	else if (PreviusMovementMode == MOVE_Swimming) {
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius()
			, DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	}
	if (PreviusMovementMode == MOVE_Custom &&
		PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		ResetLadderAttach();

		CurrentLadder = nullptr;
		UnblockMovement(EMovementBlockReason::LadderAttach);
		LadderInput = 0.0f;
	}
	if (PreviusMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline) {
		CurrentZipline = nullptr;
	}
	if (MovementMode == MOVE_Custom) {
		switch (CustomMovementMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Mantling: {
			GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_RunWall: {
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_Zipline: {
			break;
		}case (uint8)ECustomMovementMode::CMOVE_Slide: {
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_None: {
			break;
		}
		case (uint8)ECustomMovementMode::CMove_Max: {
			break;
		}

		default:
			break;
		}
	}


}

void UGCBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode) {
	case (uint8)ECustomMovementMode::CMOVE_Mantling: {
		PhysMantling(DeltaTime, Iterations);
		break;
	}
	case  (uint8)ECustomMovementMode::CMOVE_Ladder: {
		PhysLadder(DeltaTime, Iterations);
		break;
	}
	case  (uint8)ECustomMovementMode::CMOVE_Zipline: {
		PhysZipline(DeltaTime, Iterations);
		break;
	}
	case  (uint8)ECustomMovementMode::CMOVE_RunWall: {
		PhysRunWall(DeltaTime, Iterations);
		break;
	}
	case  (uint8)ECustomMovementMode::CMOVE_Slide: {
		PhysSlide(DeltaTime, Iterations);
		break;
	}
	default:
		Super::PhysCustom(DeltaTime, Iterations);
		break;
	}
}

void UGCBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	if (!IsValid(CurrentMantlingParameters.MantlingCurve))
	{
		EndMantle();
		return;
	}

	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	const float ElapsedTime =
		GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer)
		+ CurrentMantlingParameters.StartTime;

	const FVector MantlingCurveValue =
		CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);

	const float PositionAlpha = MantlingCurveValue.X;
	const float XYCorrectionAlpha = MantlingCurveValue.Y;
	const float ZCorrectionAlpha = MantlingCurveValue.Z;

	const FVector TargetLocation = GetCurrentMantlingTargetLocation();
	const FVector InitialAnimationLocation = GetCurrentMantlingInitialAnimationLocation();
	const FVector MovingTargetOffset = GetCurrentMantlingTargetOffset();

	const FVector InitialLocation =
		CurrentMantlingParameters.InitialLocation + MovingTargetOffset;

	FVector CorrectedInitialLocation = FMath::Lerp(
		InitialLocation,
		InitialAnimationLocation,
		XYCorrectionAlpha
	);

	CorrectedInitialLocation.Z = FMath::Lerp(
		InitialLocation.Z,
		InitialAnimationLocation.Z,
		ZCorrectionAlpha
	);

	const FVector NewLocation = FMath::Lerp(
		CorrectedInitialLocation,
		TargetLocation,
		PositionAlpha
	);

	const FQuat NewRotation = FQuat::Slerp(
		CurrentMantlingParameters.InitialRotation.Quaternion(),
		CurrentMantlingParameters.TargetRotation.Quaternion(),
		PositionAlpha
	);

	const FVector Delta = NewLocation - UpdatedComponent->GetComponentLocation();

	FHitResult Hit;
	SafeMoveUpdatedComponent(
		Delta,
		NewRotation,
		false,
		Hit
	);
}

void UGCBaseCharacterMovementComponent::PhysRunWall(float DeltaTime, int32 Iterations) {
	const FRunWallDescription CurrentRunWallDescription = RunWallDetectorComponent->DetectionWall();
	if (!CurrentRunWallDescription.CanRun) {
		EndRunningOnWall(EDetachFromRunWallMethod::Fall);
		return;
	}
	GetOwner()->SetActorLocation(CurrentRunWallDescription.Location);
	GetOwner()->SetActorRotation(CurrentRunWallDescription.Rotation);
	FVector Delta = DeltaTime * CurrentRunWallDescription.Velocity;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
}

void UGCBaseCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations) {
	FVector Delta = DeltaTime * GetOwner()->GetActorForwardVector() * SlideSpeed;
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	if (GCTraceUtils::OverlapCapsuleAnyByProfile(GetWorld(), GetActorLocation() + (GetOwner()->GetActorForwardVector() * 150.0f), GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCharacterOwner()->GetActorRotation().Quaternion(), FName("Visibility"), QueryParams, true, 10, FColor::Red)) {
		SlideStop();
	}
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
}
void UGCBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations) {
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = CurrentZipline->GetEndLocationMove();
	FQuat WorldSocketRotation = FQuat();
	FVector NewLocation = FMath::VInterpConstantTo(StartLocation, EndLocation, DeltaTime * 3, ZiplineSpeed) - GetOwner()->GetActorLocation();
	if ((GetOwner()->GetActorLocation() - EndLocation).IsNearlyZero(15.f)) {
		DetachFromZipline(EDetachFromLadderMethod::Fall);
		return;
	}
	FHitResult Hit;
	SafeMoveUpdatedComponent(NewLocation, GetOwner()->GetActorRotation(), false, Hit);
}
void UGCBaseCharacterMovementComponent::SetCanMoveOnLadder(bool bCanMove) {
	bCanMoveOnLadder = bCanMove;
}

void UGCBaseCharacterMovementComponent::FinishLadderAttach()
{
	// This method can be called by AnimNotify. Do not snap to target here if the
	// procedural attach movement is still in progress, otherwise the notify can
	// create a visible teleport.
	if (LadderAttachState.bInProgress)
	{
		UE_LOG(LogLadderAttach, Warning, TEXT(
			"FinishLadderAttach ignored because attach is still in progress | Character=%s | Current=%s | Target=%s | Distance=%.2f"
		),
			*GetNameSafe(GetOwner()),
			UpdatedComponent ? *UpdatedComponent->GetComponentLocation().ToString() : TEXT("None"),
			*LadderAttachState.TargetLocation.ToString(),
			UpdatedComponent ? FVector::Distance(UpdatedComponent->GetComponentLocation(), LadderAttachState.TargetLocation) : 0.0f
		);

		return;
	}

	UnblockMovement(EMovementBlockReason::LadderAttach);

	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	LadderInput = 0.0f;
}
void UGCBaseCharacterMovementComponent::UpdateLadderAttach(float DeltaTime)
{
	if (!UpdatedComponent || !IsValid(CurrentLadder))
	{
		ResetLadderAttach();
		return;
	}

	const FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderLocation = CurrentLadder->GetActorLocation();

	FVector DesiredLocation = CurrentLocation;
	bool bFinishAttachThisTick = false;

	if (!LadderAttachState.bHorizontalMoveCompleted)
	{
		const float CurrentProjection =
			FVector::DotProduct(CurrentLocation - LadderLocation, LadderUpVector);

		const float TargetProjection =
			FVector::DotProduct(LadderAttachState.TargetLocation - LadderLocation, LadderUpVector);

		const FVector HorizontalTargetLocation =
			LadderAttachState.TargetLocation
			- TargetProjection * LadderUpVector
			+ CurrentProjection * LadderUpVector;

		DesiredLocation = FMath::VInterpConstantTo(
			CurrentLocation,
			HorizontalTargetLocation,
			DeltaTime,
			LadderAttachSettings.HorizontalSpeed
		);

		const FVector HorizontalDelta =
			FVector::VectorPlaneProject(
				HorizontalTargetLocation - DesiredLocation,
				LadderUpVector
			);

		UE_LOG(LogLadderAttach, Warning, TEXT(
			"AttachPhase HORIZONTAL | Character=%s | Current=%s | HorizontalTarget=%s | Desired=%s | HorizontalDistance=%.2f | Speed=%.2f | DeltaTime=%.4f"
		),
			*GetNameSafe(GetOwner()),
			*CurrentLocation.ToString(),
			*HorizontalTargetLocation.ToString(),
			*DesiredLocation.ToString(),
			HorizontalDelta.Size(),
			LadderAttachSettings.HorizontalSpeed,
			DeltaTime
		);

		if (HorizontalDelta.Size() <= LadderAttachSettings.PhaseTolerance)
		{
			DesiredLocation = HorizontalTargetLocation;
			LadderAttachState.bHorizontalMoveCompleted = true;

			const float RemainingVertical =
				FMath::Abs(FVector::DotProduct(
					LadderAttachState.TargetLocation - DesiredLocation,
					LadderUpVector
				));

			UE_LOG(LogLadderAttach, Warning, TEXT(
				"AttachPhase SWITCH HORIZONTAL -> VERTICAL | Character=%s | Desired=%s | Target=%s | RemainingVertical=%.2f | Tolerance=%.2f"
			),
				*GetNameSafe(GetOwner()),
				*DesiredLocation.ToString(),
				*LadderAttachState.TargetLocation.ToString(),
				RemainingVertical,
				LadderAttachSettings.PhaseTolerance
			);
		}
	}
	else
	{
		const float CurrentProjection =
			FVector::DotProduct(CurrentLocation - LadderLocation, LadderUpVector);

		const float TargetProjection =
			FVector::DotProduct(LadderAttachState.TargetLocation - LadderLocation, LadderUpVector);

		const float NewProjection = FMath::FInterpConstantTo(
			CurrentProjection,
			TargetProjection,
			DeltaTime,
			LadderAttachSettings.VerticalSpeed
		);

		const FVector TargetHorizontalLocation =
			LadderAttachState.TargetLocation
			- TargetProjection * LadderUpVector;

		DesiredLocation =
			TargetHorizontalLocation
			+ NewProjection * LadderUpVector;

		const float VerticalDistance =
			FMath::Abs(TargetProjection - NewProjection);

		UE_LOG(LogLadderAttach, Warning, TEXT(
			"AttachPhase VERTICAL | Character=%s | CurrentProjection=%.2f | TargetProjection=%.2f | NewProjection=%.2f | VerticalDistance=%.2f | Current=%s | Desired=%s | Target=%s | Speed=%.2f | DeltaTime=%.4f"
		),
			*GetNameSafe(GetOwner()),
			CurrentProjection,
			TargetProjection,
			NewProjection,
			VerticalDistance,
			*CurrentLocation.ToString(),
			*DesiredLocation.ToString(),
			*LadderAttachState.TargetLocation.ToString(),
			LadderAttachSettings.VerticalSpeed,
			DeltaTime
		);

		if (VerticalDistance <= LadderAttachSettings.PhaseTolerance)
		{
			DesiredLocation = LadderAttachState.TargetLocation;
			bFinishAttachThisTick = true;

			UE_LOG(LogLadderAttach, Warning, TEXT(
				"AttachPhase FINISHED BY VERTICAL | Character=%s | FinalDesired=%s | Target=%s | Tolerance=%.2f"
			),
				*GetNameSafe(GetOwner()),
				*DesiredLocation.ToString(),
				*LadderAttachState.TargetLocation.ToString(),
				LadderAttachSettings.PhaseTolerance
			);
		}
	}

	const float RotationAlpha = FMath::Clamp(
		DeltaTime * LadderAttachSettings.RotationInterpSpeed,
		0.0f,
		1.0f
	);

	const FQuat NewRotation = FQuat::Slerp(
		UpdatedComponent->GetComponentQuat(),
		LadderAttachState.TargetRotation.Quaternion(),
		RotationAlpha
	);

	const FVector Delta = DesiredLocation - CurrentLocation;

	FHitResult Hit;

	UE_LOG(LogLadderAttach, Warning, TEXT(
		"AttachMove APPLY | Character=%s | Before=%s | Desired=%s | Delta=%s | DeltaSize=%.2f | RotationYaw=%.2f"
	),
		*GetNameSafe(GetOwner()),
		*CurrentLocation.ToString(),
		*DesiredLocation.ToString(),
		*Delta.ToString(),
		Delta.Size(),
		NewRotation.Rotator().Yaw
	);

	SafeMoveUpdatedComponent(
		Delta,
		NewRotation.Rotator(),
		false,
		Hit
	);

	UE_LOG(LogLadderAttach, Warning, TEXT(
		"AttachMove AFTER | Character=%s | After=%s | Hit=%d | BlockingHit=%d | HitActor=%s"
	),
		*GetNameSafe(GetOwner()),
		*UpdatedComponent->GetComponentLocation().ToString(),
		Hit.IsValidBlockingHit() ? 1 : 0,
		Hit.bBlockingHit ? 1 : 0,
		*GetNameSafe(Hit.GetActor())
	);

	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	LadderInput = 0.0f;

	if (bFinishAttachThisTick)
	{
		ResetLadderAttach();
		UnblockMovement(EMovementBlockReason::LadderAttach);
	}
}

void UGCBaseCharacterMovementComponent::PhysLadderMovement(float DeltaTime)
{
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();

	const float CurrentLadderInput =
		IsMovementBlockedBy(EMovementBlockReason::LadderAttach)
		? 0.0f
		: LadderInput;

	Acceleration = FVector::ZeroVector;

	Velocity =
		LadderUpVector
		* CurrentLadderInput
		* ClimbingOnLadderMaxSpeed;

	const FVector Delta = Velocity * DeltaTime;

	const FVector NewPos =
		UpdatedComponent
		? UpdatedComponent->GetComponentLocation() + Delta
		: GetOwner()->GetActorLocation() + Delta;

	const float NewPosProjection =
		GetActorToCurrentLadderProjection(NewPos);

	const bool bMovingUp = CurrentLadderInput > 0.0f;
	const bool bMovingDown = CurrentLadderInput < 0.0f;

	FHitResult Hit;

	if (NewPosProjection < MinLadderBottomOffset)
	{
		if (bMovingDown)
		{
			LadderInput = 0.0f;
			Velocity = FVector::ZeroVector;

			UE_LOG(LogLadderAttach, Warning, TEXT(
				"PhysLadder DETACH BOTTOM | Character=%s | Projection=%.2f | MinBottom=%.2f"
			),
				*GetNameSafe(GetOwner()),
				NewPosProjection,
				MinLadderBottomOffset
			);

			DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
			return;
		}
	}
	else if (NewPosProjection > CurrentLadder->GetLadderHeight() - MaxLadderTopOffset)
	{
		if (bMovingUp)
		{
			LadderInput = 0.0f;
			Velocity = FVector::ZeroVector;

			UE_LOG(LogLadderAttach, Warning, TEXT(
				"PhysLadder DETACH TOP | Character=%s | Projection=%.2f | TopLimit=%.2f | LadderHeight=%.2f"
			),
				*GetNameSafe(GetOwner()),
				NewPosProjection,
				CurrentLadder->GetLadderHeight() - MaxLadderTopOffset,
				CurrentLadder->GetLadderHeight()
			);

			DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
			return;
		}
	}

	SafeMoveUpdatedComponent(
		Delta,
		GetOwner()->GetActorRotation(),
		true,
		Hit
	);

	LadderInput = 0.0f;
}

void UGCBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	if (!IsValid(CurrentLadder))
	{
		ResetLadderAttach();
		SetMovementMode(MOVE_Falling);
		return;
	}

	if (LadderAttachState.bInProgress)
	{
		UpdateLadderAttach(DeltaTime);
		return;
	}

	PhysLadderMovement(DeltaTime);
}

FVector UGCBaseCharacterMovementComponent::GetCurrentMantlingTargetLocation() const
{
	if (CurrentMantlingParameters.bHasMovingTarget && IsValid(CurrentMantlingParameters.TargetComponent))
	{
		return CurrentMantlingParameters.TargetComponent
			->GetComponentTransform()
			.TransformPosition(CurrentMantlingParameters.LocalTargetLocation);
	}

	return CurrentMantlingParameters.TargetLocation;
}

FVector UGCBaseCharacterMovementComponent::GetCurrentMantlingInitialAnimationLocation() const
{
	if (CurrentMantlingParameters.bHasMovingTarget && IsValid(CurrentMantlingParameters.TargetComponent))
	{
		return CurrentMantlingParameters.TargetComponent->GetComponentTransform()
			.TransformPosition(CurrentMantlingParameters.LocalInitialAnimationLocation);
	}

	return CurrentMantlingParameters.InitialAnimationLocation;
}

FVector UGCBaseCharacterMovementComponent::GetCurrentMantlingTargetOffset() const
{
	if (!CurrentMantlingParameters.bHasMovingTarget || !IsValid(CurrentMantlingParameters.TargetComponent))
	{
		return FVector::ZeroVector;
	}

	const FVector StartLocation =
		CurrentMantlingParameters.TargetComponentStartTransform.GetLocation();

	const FVector CurrentLocation =
		CurrentMantlingParameters.TargetComponent->GetComponentLocation();

	return CurrentLocation - StartLocation;
}
void UGCBaseCharacterMovementComponent::BlockMovement(EMovementBlockReason Reason)
{
	MovementBlockReason = Reason;

	LadderInput = 0.0f;
	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;

	ConsumeInputVector();

	if (CharacterOwner)
	{
		CharacterOwner->ConsumeMovementInputVector();
	}
}
void UGCBaseCharacterMovementComponent::UnblockMovement(EMovementBlockReason Reason)
{
	if (MovementBlockReason == Reason)
	{
		MovementBlockReason = EMovementBlockReason::None;
	}
}
bool UGCBaseCharacterMovementComponent::IsMovementBlocked() const
{
	return MovementBlockReason != EMovementBlockReason::None;
}
bool UGCBaseCharacterMovementComponent::IsMovementBlockedBy(EMovementBlockReason Reason) const
{
	return MovementBlockReason == Reason;
}
void FSavedMove_GC::Clear()
{
	Super::Clear();
	bSavedIsSprinting = 0;
}

uint8 FSavedMove_GC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	/*FLAG_JumpPressed = 0x01,	// Jump pressed
			FLAG_WantsToCrouch = 0x02,	// Wants to crouch
			FLAG_Reserved_1 = 0x04,	// Reserved for future use
			FLAG_Reserved_2 = 0x08,	// Reserved for future use
			// Remaining bit masks are available for custom flags.
			FLAG_Custom_0 = 0x10, -Sprinting flag
			FLAG_Custom_1 = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,*/
	if (bSavedIsSprinting) {
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool FSavedMove_GC::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_GC* NewMove = StaticCast<const FSavedMove_GC*>(NewMovePtr.Get());
	if (bSavedIsSprinting != NewMove->bSavedIsSprinting) {
		return false;
	}
	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

void FSavedMove_GC::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientDataCharacter)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientDataCharacter);
	checkf(Character->GetCharacterMovement()->IsA<UGCBaseCharacterMovementComponent>(), TEXT("FSavedMove_GC::SetMoveFor CharacterMovement is not UGCBaseCharacterMovementComponent"));
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(Character->GetCharacterMovement());
	bSavedIsSprinting = MovementComponent->IsSprinting();

}
void FSavedMove_GC::PrepMoveFor(ACharacter* Character) {
	Super::PrepMoveFor(Character);
	checkf(Character->GetCharacterMovement()->IsA<UGCBaseCharacterMovementComponent>(), TEXT("FSavedMove_GC::PrepMoveFor CharacterMovement is not UGCBaseCharacterMovementComponent"));
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(Character->GetMovementComponent());
	MovementComponent->SetIsSprinting(bSavedIsSprinting);

}

FNetworkPredictionData_Client_Character_GC::FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{}

FSavedMovePtr FNetworkPredictionData_Client_Character_GC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GC());
}
