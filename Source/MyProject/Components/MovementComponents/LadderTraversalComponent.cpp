// Fill out your copyright notice in the Description page of Project Settings.

#include "LadderTraversalComponent.h"
#include "MyProject.h"
#include "../../Actors/Interactive/Environment/Ladder.h"

DEFINE_LOG_CATEGORY_STATIC(LogLadderTraversal, Log, All);

ULadderTraversalComponent::ULadderTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULadderTraversalComponent::Initialize(UGCBaseCharacterMovementComponent* InMovementComponent)
{
	MovementComponent = InMovementComponent;
}


bool ULadderTraversalComponent::CanAttachToLadder() const
{
	const UWorld* World = MovementComponent ? MovementComponent->GetWorld() : GetWorld();
	if (!World)
	{
		return true;
	}

	return World->GetTimeSeconds() - LastDetachTime >= ReattachCooldown;
}

bool ULadderTraversalComponent::AttachToLadder(
	const ALadder* Ladder,
	const FVector& CurrentLocation,
	const FRotator& CurrentRotation,
	FVector& OutLocation,
	FRotator& OutRotation,
	bool& bOutAttachFromTop
)
{
	bOutAttachFromTop = false;

	if (!IsValid(Ladder))
	{
		return false;
	}

	if (!CanAttachToLadder())
	{
		UE_LOG(LogLadderTraversal, Warning, TEXT(
			"AttachToLadder blocked by cooldown | Owner=%s | Ladder=%s"
		),
			*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
			*GetNameSafe(Ladder)
		);

		return false;
	}

	CurrentLadder = Ladder;
	bOutAttachFromTop = CurrentLadder->GetIsOnTop();

	if (bOutAttachFromTop)
	{
		StartAttachFromTop(CurrentLocation, CurrentRotation);
		OutLocation = AttachState.StartLocation;
		OutRotation = AttachState.StartRotation;
		return true;
	}

	ResetAttach();

	const float ActorToLadderProjection = GetProjection(CurrentLocation);
	OutLocation = GetAlignedLocation(ActorToLadderProjection);
	OutRotation = GetTargetRotation();

	UE_LOG(LogLadderTraversal, Warning, TEXT(
		"AttachToLadder BOTTOM | Owner=%s | Location=%s | Projection=%.2f"
	),
		*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
		*OutLocation.ToString(),
		GetProjection(OutLocation)
	);

	return true;
}

void ULadderTraversalComponent::StartAttachFromTop(const FVector& CurrentLocation, const FRotator& CurrentRotation)
{
	if (!IsValid(CurrentLadder))
	{
		UE_LOG(LogLadderTraversal, Warning, TEXT("StartAttachFromTop skipped: CurrentLadder is invalid | Owner=%s"), *GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()));
		ResetAttach();
		return;
	}

	AttachState.StartLocation = CurrentLocation;
	AttachState.StartRotation = CurrentRotation;

	const float TargetProjection =
		CurrentLadder->GetLadderHeight() - AttachSettings.TopAttachStartOffset;

	AttachState.TargetLocation = GetAlignedLocation(TargetProjection);
	AttachState.TargetRotation = GetTargetRotation();
	AttachState.bInProgress = true;
	AttachState.bHorizontalMoveCompleted = false;

	UE_LOG(LogLadderTraversal, Warning, TEXT(
		"StartLadderAttachFromTop | Owner=%s | Start=%s | Target=%s | StartProjection=%.2f | TargetProjection=%.2f | LadderHeight=%.2f | TopAttachStartOffset=%.2f | HorizontalSpeed=%.2f | VerticalSpeed=%.2f"
	),
		*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
		*AttachState.StartLocation.ToString(),
		*AttachState.TargetLocation.ToString(),
		GetProjection(AttachState.StartLocation),
		GetProjection(AttachState.TargetLocation),
		CurrentLadder->GetLadderHeight(),
		AttachSettings.TopAttachStartOffset,
		AttachSettings.HorizontalSpeed,
		AttachSettings.VerticalSpeed
	);
}

void ULadderTraversalComponent::ResetAttach()
{
	AttachState.Reset();
}

void ULadderTraversalComponent::NotifyDetachedFromLadder()
{
	if (UWorld* World = MovementComponent ? MovementComponent->GetWorld() : GetWorld())
	{
		LastDetachTime = World->GetTimeSeconds();
	}

	ResetAttach();
}

void ULadderTraversalComponent::ClearCurrentLadder()
{
	ResetAttach();
	CurrentLadder = nullptr;
}

bool ULadderTraversalComponent::HasValidLadder() const
{
	return IsValid(CurrentLadder);
}

bool ULadderTraversalComponent::IsAttachInProgress() const
{
	return AttachState.bInProgress;
}

float ULadderTraversalComponent::GetProjection(const FVector& Location) const
{
	if (!IsValid(CurrentLadder))
	{
		UE_LOG(LogLadderTraversal, Verbose, TEXT("GetProjection skipped: CurrentLadder is invalid | Owner=%s"), *GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()));
		return 0.0f;
	}

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();

	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

FVector ULadderTraversalComponent::GetAlignedLocation(float LadderProjection) const
{
	if (!IsValid(CurrentLadder))
	{
		UE_LOG(LogLadderTraversal, Verbose, TEXT("GetAlignedLocation skipped: CurrentLadder is invalid | Owner=%s"), *GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()));
		return FVector::ZeroVector;
	}

	return CurrentLadder->GetActorLocation()
		+ LadderProjection * CurrentLadder->GetActorUpVector()
		+ LadderToCharacterOffset * CurrentLadder->GetActorForwardVector();
}

FRotator ULadderTraversalComponent::GetTargetRotation() const
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

float ULadderTraversalComponent::GetSpeedRatio(const FVector& Velocity) const
{
	if (!IsValid(CurrentLadder))
	{
		UE_LOG(LogLadderTraversal, Verbose, TEXT("GetSpeedRatio skipped: CurrentLadder is invalid | Owner=%s"), *GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()));
		return 0.0f;
	}

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return ClimbingSpeed > 0.0f
		? FVector::DotProduct(LadderUpVector, Velocity) / ClimbingSpeed
		: 0.0f;
}

bool ULadderTraversalComponent::BuildAttachMovement(
	float DeltaTime,
	const FVector& CurrentLocation,
	const FQuat& CurrentRotation,
	FLadderTraversalMovementFrame& OutFrame,
	bool& bOutFinished
)
{
	OutFrame = FLadderTraversalMovementFrame();
	bOutFinished = false;

	if (!IsValid(CurrentLadder) || !AttachState.bInProgress)
	{
		return false;
	}

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderLocation = CurrentLadder->GetActorLocation();

	FVector DesiredLocation = CurrentLocation;

	if (!AttachState.bHorizontalMoveCompleted)
	{
		const float CurrentProjection =
			FVector::DotProduct(CurrentLocation - LadderLocation, LadderUpVector);

		const float TargetProjection =
			FVector::DotProduct(AttachState.TargetLocation - LadderLocation, LadderUpVector);

		const FVector HorizontalTargetLocation =
			AttachState.TargetLocation
			- TargetProjection * LadderUpVector
			+ CurrentProjection * LadderUpVector;

		DesiredLocation = FMath::VInterpConstantTo(
			CurrentLocation,
			HorizontalTargetLocation,
			DeltaTime,
			AttachSettings.HorizontalSpeed
		);

		const FVector HorizontalDelta =
			FVector::VectorPlaneProject(
				HorizontalTargetLocation - DesiredLocation,
				LadderUpVector
			);

		UE_LOG(LogLadderTraversal, Verbose, TEXT(
			"AttachPhase HORIZONTAL | Owner=%s | Current=%s | HorizontalTarget=%s | Desired=%s | HorizontalDistance=%.2f | Speed=%.2f | DeltaTime=%.4f"
		),
			*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
			*CurrentLocation.ToString(),
			*HorizontalTargetLocation.ToString(),
			*DesiredLocation.ToString(),
			HorizontalDelta.Size(),
			AttachSettings.HorizontalSpeed,
			DeltaTime
		);

		if (HorizontalDelta.Size() <= AttachSettings.PhaseTolerance)
		{
			DesiredLocation = HorizontalTargetLocation;
			AttachState.bHorizontalMoveCompleted = true;

			const float RemainingVertical =
				FMath::Abs(FVector::DotProduct(
					AttachState.TargetLocation - DesiredLocation,
					LadderUpVector
				));

			UE_LOG(LogLadderTraversal, Warning, TEXT(
				"AttachPhase SWITCH HORIZONTAL -> VERTICAL | Owner=%s | Desired=%s | Target=%s | RemainingVertical=%.2f | Tolerance=%.2f"
			),
				*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
				*DesiredLocation.ToString(),
				*AttachState.TargetLocation.ToString(),
				RemainingVertical,
				AttachSettings.PhaseTolerance
			);
		}
	}
	else
	{
		const float CurrentProjection =
			FVector::DotProduct(CurrentLocation - LadderLocation, LadderUpVector);

		const float TargetProjection =
			FVector::DotProduct(AttachState.TargetLocation - LadderLocation, LadderUpVector);

		const float NewProjection = FMath::FInterpConstantTo(
			CurrentProjection,
			TargetProjection,
			DeltaTime,
			AttachSettings.VerticalSpeed
		);

		const FVector TargetHorizontalLocation =
			AttachState.TargetLocation
			- TargetProjection * LadderUpVector;

		DesiredLocation =
			TargetHorizontalLocation
			+ NewProjection * LadderUpVector;

		const float VerticalDistance =
			FMath::Abs(TargetProjection - NewProjection);

		UE_LOG(LogLadderTraversal, Verbose, TEXT(
			"AttachPhase VERTICAL | Owner=%s | CurrentProjection=%.2f | TargetProjection=%.2f | NewProjection=%.2f | VerticalDistance=%.2f | Current=%s | Desired=%s | Target=%s | Speed=%.2f | DeltaTime=%.4f"
		),
			*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
			CurrentProjection,
			TargetProjection,
			NewProjection,
			VerticalDistance,
			*CurrentLocation.ToString(),
			*DesiredLocation.ToString(),
			*AttachState.TargetLocation.ToString(),
			AttachSettings.VerticalSpeed,
			DeltaTime
		);

		if (VerticalDistance <= AttachSettings.PhaseTolerance)
		{
			DesiredLocation = AttachState.TargetLocation;
			bOutFinished = true;

			UE_LOG(LogLadderTraversal, Warning, TEXT(
				"AttachPhase FINISHED BY VERTICAL | Owner=%s | FinalDesired=%s | Target=%s | Tolerance=%.2f"
			),
				*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
				*DesiredLocation.ToString(),
				*AttachState.TargetLocation.ToString(),
				AttachSettings.PhaseTolerance
			);
		}
	}

	const float RotationAlpha = FMath::Clamp(
		DeltaTime * AttachSettings.RotationInterpSpeed,
		0.0f,
		1.0f
	);

	const FQuat NewRotation = FQuat::Slerp(
		CurrentRotation,
		AttachState.TargetRotation.Quaternion(),
		RotationAlpha
	);

	OutFrame.Delta = DesiredLocation - CurrentLocation;
	OutFrame.Rotation = NewRotation.Rotator();
	OutFrame.Velocity = FVector::ZeroVector;
	OutFrame.bSweep = false;
	OutFrame.bShouldDetach = false;

	return true;
}

bool ULadderTraversalComponent::BuildClimbMovement(
	float DeltaTime,
	const FVector& CurrentLocation,
	const FRotator& CurrentRotation,
	float Input,
	bool bInputBlocked,
	FLadderTraversalMovementFrame& OutFrame
)
{
	OutFrame = FLadderTraversalMovementFrame();

	if (!IsValid(CurrentLadder))
	{
		return false;
	}

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();

	const float CurrentLadderInput = bInputBlocked
		? 0.0f
		: FMath::Clamp(Input, -1.0f, 1.0f);

	OutFrame.Velocity = LadderUpVector * CurrentLadderInput * ClimbingSpeed;
	OutFrame.Delta = OutFrame.Velocity * DeltaTime;
	OutFrame.Rotation = CurrentRotation;
	OutFrame.bSweep = true;

	const FVector NewPos = CurrentLocation + OutFrame.Delta;
	const float NewPosProjection = GetProjection(NewPos);

	const bool bMovingUp = CurrentLadderInput > 0.0f;
	const bool bMovingDown = CurrentLadderInput < 0.0f;

	if (NewPosProjection < MinBottomOffset && bMovingDown)
	{
		UE_LOG(LogLadderTraversal, Warning, TEXT(
			"PhysLadder DETACH BOTTOM | Owner=%s | Projection=%.2f | MinBottom=%.2f"
		),
			*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
			NewPosProjection,
			MinBottomOffset
		);

		OutFrame.Delta = FVector::ZeroVector;
		OutFrame.Velocity = FVector::ZeroVector;
		OutFrame.bShouldDetach = true;
		OutFrame.DetachMethod = EDetachFromLadderMethod::ReachingTheBottom;
		return true;
	}

	if (NewPosProjection > CurrentLadder->GetLadderHeight() - MaxTopOffset && bMovingUp)
	{
		UE_LOG(LogLadderTraversal, Warning, TEXT(
			"PhysLadder DETACH TOP | Owner=%s | Projection=%.2f | TopLimit=%.2f | LadderHeight=%.2f"
		),
			*GetNameSafe(MovementComponent ? MovementComponent->GetOwner() : GetOwner()),
			NewPosProjection,
			CurrentLadder->GetLadderHeight() - MaxTopOffset,
			CurrentLadder->GetLadderHeight()
		);

		OutFrame.Delta = FVector::ZeroVector;
		OutFrame.Velocity = FVector::ZeroVector;
		OutFrame.bShouldDetach = true;
		OutFrame.DetachMethod = EDetachFromLadderMethod::ReachingTheTop;
		return true;
	}

	return true;
}
