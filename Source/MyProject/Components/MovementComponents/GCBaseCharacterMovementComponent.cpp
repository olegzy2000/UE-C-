// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacterMovementComponent.h"
#include "MyProject.h"
#include "LadderTraversalComponent.h"
#include "../../Characters/GCBaseCharacter.h"
#include "../../Utils/GCTraceUtils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"


UGCBaseCharacterMovementComponent::UGCBaseCharacterMovementComponent()
{
	RunWallDetectorComponent = CreateDefaultSubobject<URunWallDetectorComponent>(TEXT("RunWallDetectorComponent"));
	LadderTraversalComponent = CreateDefaultSubobject<ULadderTraversalComponent>(TEXT("LadderTraversalComponent"));

	if (LadderTraversalComponent)
	{
		LadderTraversalComponent->Initialize(this);
	}
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
	else if (IsOnLadder() && LadderTraversalComponent) {
		Result = LadderTraversalComponent->GetClimbingSpeed();
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


bool UGCBaseCharacterMovementComponent::IsLadderAttachInProgress() const
{
	return LadderTraversalComponent && LadderTraversalComponent->IsAttachInProgress();
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	if (!IsValid(Ladder) || !LadderTraversalComponent)
	{
		return;
	}


	FVector NewCharacterLocation = GetOwner()->GetActorLocation();
	FRotator NewCharacterRotation = GetOwner()->GetActorRotation();
	bool bAttachFromTop = false;

	const bool bAttached = LadderTraversalComponent->AttachToLadder(
		Ladder,
		UpdatedComponent ? UpdatedComponent->GetComponentLocation() : GetOwner()->GetActorLocation(),
		UpdatedComponent ? UpdatedComponent->GetComponentRotation() : GetOwner()->GetActorRotation(),
		NewCharacterLocation,
		NewCharacterRotation,
		bAttachFromTop
	);

	if (!bAttached)
	{
		return;
	}

	if (bAttachFromTop)
	{
		BlockMovement(EMovementBlockReason::LadderAttach);

		StopMovementImmediately();
		StopActiveMovement();
		ClearAccumulatedForces();
	}
	else
	{
		UnblockMovement(EMovementBlockReason::LadderAttach);
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(NewCharacterRotation);

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
	if (!IsValid(Zipline) || !IsValid(GetBaseCharacterOwner()))
	{
		UE_LOG(LogTraversal, Warning, TEXT("AttachToZipline skipped: invalid zipline or character owner | Owner=%s | Zipline=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(Zipline));
		return;
	}

	if (!IsValid(Zipline->GetLeftRailMeshComponent()) || !IsValid(Zipline->GetRightRailMeshComponent()))
	{
		UE_LOG(LogTraversal, Warning, TEXT("AttachToZipline skipped: zipline rail mesh is invalid | Zipline=%s"), *GetNameSafe(Zipline));
		return;
	}

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
	if (!LadderTraversalComponent || !LadderTraversalComponent->HasValidLadder())
	{
		UE_LOG(LogTraversal, Verbose, TEXT("GetActorToCurrentLadderProjection skipped: ladder is invalid | Owner=%s"), *GetNameSafe(GetOwner()));
		return 0.0f;
	}

	return LadderTraversalComponent->GetProjection(Location);
}
void UGCBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	const ALadder* Ladder = GetCurrentLadder();

	if (LadderTraversalComponent)
	{
		LadderTraversalComponent->NotifyDetachedFromLadder();
	}

	LadderInput = 0.0f;
	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	UnblockMovement(EMovementBlockReason::LadderAttach);

	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(EMovementMode::MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::JumpOff:
	{
		const FVector JumpDirection = IsValid(Ladder)
			? Ladder->GetActorForwardVector()
			: GetOwner()->GetActorForwardVector();

		SetMovementMode(EMovementMode::MOVE_Falling);

		const float JumpSpeed = LadderTraversalComponent
			? LadderTraversalComponent->GetJumpOffFromLadderSpeed()
			: 500.0f;

		const FVector JumpVelocity = JumpDirection * JumpSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::Fall:
	default:
	{
		SetMovementMode(EMovementMode::MOVE_Falling);
		break;
	}
	}
}
void UGCBaseCharacterMovementComponent::DetachFromZipline(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::JumpOff: {
		FVector JumpDirection = CharacterOwner->GetActorRotation().Vector();
		SetMovementMode(EMovementMode::MOVE_Falling);
		const float JumpSpeed = LadderTraversalComponent
			? LadderTraversalComponent->GetJumpOffFromLadderSpeed()
			: 500.0f;

		FVector JumpVelocity = JumpDirection * JumpSpeed;
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
	return LadderTraversalComponent ? LadderTraversalComponent->GetCurrentLadder() : nullptr;
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

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
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
	if (!LadderTraversalComponent || !LadderTraversalComponent->HasValidLadder())
	{
		UE_LOG(LogTraversal, Verbose, TEXT("GetLadderSpeedRation skipped: ladder is invalid | Owner=%s"), *GetNameSafe(GetOwner()));
		return 0.0f;
	}

	return LadderTraversalComponent->GetSpeedRatio(Velocity);
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
		if (LadderTraversalComponent)
		{
			LadderTraversalComponent->ClearCurrentLadder();
		}

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
	if (LadderTraversalComponent && LadderTraversalComponent->IsAttachInProgress())
	{
		UE_LOG(LogLadderAttach, Warning, TEXT(
			"FinishLadderAttach ignored because attach is still in progress | Character=%s"
		),
			*GetNameSafe(GetOwner())
		);

		return;
	}

	UnblockMovement(EMovementBlockReason::LadderAttach);

	Velocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
	LadderInput = 0.0f;
}

void UGCBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	if (!LadderTraversalComponent)
	{
		SetMovementMode(MOVE_Falling);
		return;
	}


	if (!LadderTraversalComponent->HasValidLadder())
	{
		SetMovementMode(MOVE_Falling);
		return;
	}

	const FVector CurrentLocation = UpdatedComponent
		? UpdatedComponent->GetComponentLocation()
		: GetOwner()->GetActorLocation();

	if (LadderTraversalComponent->IsAttachInProgress())
	{
		FLadderTraversalMovementFrame Frame;
		bool bAttachFinished = false;

		if (!LadderTraversalComponent->BuildAttachMovement(
			DeltaTime,
			CurrentLocation,
			UpdatedComponent ? UpdatedComponent->GetComponentQuat() : GetOwner()->GetActorQuat(),
			Frame,
			bAttachFinished
		))
		{
			return;
		}

		FHitResult Hit;
		SafeMoveUpdatedComponent(
			Frame.Delta,
			Frame.Rotation,
			Frame.bSweep,
			Hit
		);

		Velocity = FVector::ZeroVector;
		Acceleration = FVector::ZeroVector;
		LadderInput = 0.0f;

		if (bAttachFinished)
		{
			LadderTraversalComponent->ResetAttach();
			UnblockMovement(EMovementBlockReason::LadderAttach);
		}

		return;
	}

	FLadderTraversalMovementFrame Frame;
	if (!LadderTraversalComponent->BuildClimbMovement(
		DeltaTime,
		CurrentLocation,
		GetOwner()->GetActorRotation(),
		LadderInput,
		IsMovementBlockedBy(EMovementBlockReason::LadderAttach),
		Frame
	))
	{
		SetMovementMode(MOVE_Falling);
		return;
	}

	Velocity = Frame.Velocity;
	Acceleration = FVector::ZeroVector;

	if (Frame.bShouldDetach)
	{
		LadderInput = 0.0f;
		Velocity = FVector::ZeroVector;
		DetachFromLadder(Frame.DetachMethod);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(
		Frame.Delta,
		Frame.Rotation,
		Frame.bSweep,
		Hit
	);

	LadderInput = 0.0f;
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

	UGCBaseCharacterMovementComponent* MovementComponent = IsValid(Character)
		? Cast<UGCBaseCharacterMovementComponent>(Character->GetCharacterMovement())
		: nullptr;

	if (!IsValid(MovementComponent))
	{
		bSavedIsSprinting = false;
		UE_LOG(LogTraversal, Warning, TEXT("FSavedMove_GC::SetMoveFor skipped: CharacterMovement is not UGCBaseCharacterMovementComponent | Character=%s"), *GetNameSafe(Character));
		return;
	}

	bSavedIsSprinting = MovementComponent->IsSprinting();
}
void FSavedMove_GC::PrepMoveFor(ACharacter* Character) {
	Super::PrepMoveFor(Character);

	UGCBaseCharacterMovementComponent* MovementComponent = IsValid(Character)
		? Cast<UGCBaseCharacterMovementComponent>(Character->GetMovementComponent())
		: nullptr;

	if (!IsValid(MovementComponent))
	{
		UE_LOG(LogTraversal, Warning, TEXT("FSavedMove_GC::PrepMoveFor skipped: CharacterMovement is not UGCBaseCharacterMovementComponent | Character=%s"), *GetNameSafe(Character));
		return;
	}

	MovementComponent->SetIsSprinting(bSavedIsSprinting);
}

FNetworkPredictionData_Client_Character_GC::FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{}

FSavedMovePtr FNetworkPredictionData_Client_Character_GC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GC());
}
