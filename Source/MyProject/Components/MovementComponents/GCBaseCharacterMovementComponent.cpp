// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacterMovementComponent.h"
#include "../../Characters/GCBaseCharacter.h"
#include "../../Utils/GCTraceUtils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

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
bool UGCBaseCharacterMovementComponent::IsSprinting()
{
	return bIsSprinting;
}
void UGCBaseCharacterMovementComponent::SetIsSprinting(bool flag)
{
	bIsSprinting=flag;
}

void UGCBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bIsSprinting = (Flags &= FSavedMove_Character::FLAG_Custom_0)!=0;
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
	else if (GetBaseCharacterOwner()->IsAming()) {
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
	if(IsSlide()){
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
	GetCharacterOwner()->GetMesh()->MoveComponent(FVector(0.f, 0.f, -(SlideCaspsuleHalfHeight - 88)-10), GetCharacterOwner()->GetMesh()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	FTimerHandle TimerHandleSlideStop;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleSlideStop,this,&UGCBaseCharacterMovementComponent::SlideStop, SlideMaxTime, false);
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



void UGCBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float ActorToLadderProjection = GetActorToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + ActorToLadderProjection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;
	if (CurrentLadder->GetIsOnTop()) {
		NewCharacterLocation = CurrentLadder->GetAnimMontageStartingLocation();
	}
	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	SetMovementMode(EMovementMode::MOVE_Custom,(uint8) ECustomMovementMode::CMOVE_Ladder);
}


void UGCBaseCharacterMovementComponent::AttachToZipline(AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FRotator TargetOrientationRotation = CurrentZipline->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;
	FVector StartLocation;
	FVector EndLocation ;
	FQuat WorldSocketRotation = FQuat();
	CurrentZipline->GetLeftRailMeshComponent()->GetSocketWorldLocationAndRotation(FName("AttachPoint"), StartLocation, WorldSocketRotation);
	CurrentZipline->GetRightRailMeshComponent()->GetSocketWorldLocationAndRotation(FName("AttachPoint"), EndLocation, WorldSocketRotation);
	FVector ToEndPoint = EndLocation - GetBaseCharacterOwner()->GetActorLocation();
	FVector ToStartPoint = StartLocation - GetBaseCharacterOwner()->GetActorLocation();
	FVector FromStartToEnd = EndLocation - StartLocation;
	FVector StartCharacterLocation = StartLocation- UKismetMathLibrary::ProjectVectorOnToVector(ToStartPoint, FromStartToEnd);
	GetBaseCharacterOwner()->SetActorLocation(FVector(StartCharacterLocation.X,StartCharacterLocation.Y,StartCharacterLocation.Z- ZiplineOffset));
	if (((acosf(FVector::DotProduct(GetBaseCharacterOwner()->GetActorRotation().Vector(), ToStartPoint.ToOrientationRotator().Vector()))) * (180 / 3.1415926)) < ((acosf(FVector::DotProduct(GetBaseCharacterOwner()->GetActorRotation().Vector(), ToEndPoint.ToOrientationRotator().Vector()))) * (180 / 3.1415926))) {
		CurrentZipline->SetEndLocationMove(StartLocation-FVector(0,0, ZiplineOffset));
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
	case EDetachFromLadderMethod::ReachingTheBottom : {
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
	FRotator finalRotator=CharacterOwner->GetActorRotation();
	CharacterOwner->SetActorRotation(FRotator(0.0f,finalRotator.Yaw,finalRotator.Roll));
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
			if(RunWallDescription.bIsLeft)
			JumpDirection = GetOwner()->GetActorRightVector()+GetOwner()->GetActorForwardVector();
			else
			JumpDirection = -GetOwner()->GetActorRightVector() + GetOwner()->GetActorForwardVector();
			SetMovementMode(EMovementMode::MOVE_Falling);
			FVector JumpVelocity = (JumpDirection) * JumpOffFromRunWall;
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
	else if(!IsOnLadder())
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
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius,SwimmingCapsuleHalfSize);
	}
	else if (PreviusMovementMode == MOVE_Swimming) {
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius()
			, DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	}
	if (PreviousCustomMode== MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder) {
		CurrentLadder = nullptr;
	}
	if (PreviousCustomMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline) {
		CurrentZipline = nullptr;
	}
	if (MovementMode == MOVE_Custom) {
		switch (CustomMovementMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:{
			GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration,false);
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
		break;
	}
	Super::PhysCustom(DeltaTime, Iterations);
}

void UGCBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations) {
	if (CurrentMantlingParameters.MantlingCurve != nullptr) {
		float ElapseTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
		FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapseTime);
		float PositionAlpha = MantlingCurveValue.X;
		float XYCorrectionAlpha = MantlingCurveValue.Y;
		float ZCorrectionAlpha = MantlingCurveValue.Z;
		FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
		CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
		FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);

		FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);
		FVector Delta = NewLocation - GetActorLocation();
		if (CurrentMantlingParameters.HitObject != NULL) {
			//	Delta -= CurrentMantlingParameters.HitObject->GetPlatformMesh()->GetComponentLocation();
		}
		//Velocity = Delta / DeltaTime;
		//GetOwner()->AttachToComponent(CurrentMantlingParameters.HitObject->GetPlatformMesh(), FAttachmentTransformRules::KeepWorldTransform);
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
	}
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
	FVector Delta = DeltaTime * GetOwner()->GetActorForwardVector()* SlideSpeed;
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	if (GCTraceUtils::OverlapCapsuleAnyByProfile(GetWorld(),  GetActorLocation() + (GetOwner()->GetActorForwardVector() * 150.0f),GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),GetCharacterOwner()->GetActorRotation().Quaternion(),FName("Visibility"), QueryParams, true, 10, FColor::Red)) {
		SlideStop();
	}
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
}
void UGCBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations) {
	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation=CurrentZipline->GetEndLocationMove();
	FQuat WorldSocketRotation = FQuat();
	FVector NewLocation= FMath::VInterpConstantTo(StartLocation, EndLocation, DeltaTime*3, ZiplineSpeed)-GetOwner()->GetActorLocation();
	if ((GetOwner()->GetActorLocation()-EndLocation).IsNearlyZero(15.f)) {
		DetachFromZipline(EDetachFromLadderMethod::Fall);
		return;
	}
	FHitResult Hit;
	SafeMoveUpdatedComponent(NewLocation, GetOwner()->GetActorRotation(), false, Hit);
}
void UGCBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations) {
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBreakingDecelaration);
	FVector Delta = Velocity * DeltaTime;
	if (HasAnimRootMotion()) {
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}
	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);
	if (NewPosProjection < MinLadderBottomOffset) {
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset)) {
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta,GetOwner()->GetActorRotation() , true, Hit);
}

void FSavedMove_GC::Clear()
{
	Super::Clear();
	bSavedIsSprinting = 0;
}

uint8 FSavedMove_GC::GetCompressedFalgs() const
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
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(Character->GetCharacterMovement());
	bSavedIsSprinting = MovementComponent->IsSprinting();

}
void FSavedMove_GC::PrepMoveFor(ACharacter* Character) {
	Super::PrepMoveFor(Character);
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(Character->GetMovementComponent());
	MovementComponent->SetIsSprinting(bSavedIsSprinting);

}

FNetworkPredictionData_Client_Character_GC::FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_Character_GC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GC());
}
