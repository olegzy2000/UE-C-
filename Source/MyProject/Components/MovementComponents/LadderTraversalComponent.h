// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GCBaseCharacterMovementComponent.h"
#include "LadderTraversalComponent.generated.h"

class ALadder;
class UGCBaseCharacterMovementComponent;

USTRUCT()
struct FLadderAttachMovementState
{
	GENERATED_BODY()

	bool bInProgress = false;
	bool bHorizontalMoveCompleted = false;

	FVector StartLocation = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;

	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator TargetRotation = FRotator::ZeroRotator;

	void Reset()
	{
		bInProgress = false;
		bHorizontalMoveCompleted = false;

		StartLocation = FVector::ZeroVector;
		TargetLocation = FVector::ZeroVector;

		StartRotation = FRotator::ZeroRotator;
		TargetRotation = FRotator::ZeroRotator;
	}
};

USTRUCT(BlueprintType)
struct FLadderAttachMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float TopAttachStartOffset = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float HorizontalSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float VerticalSpeed = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PhaseTolerance = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float RotationInterpSpeed = 8.0f;
};

USTRUCT()
struct FLadderTraversalMovementFrame
{
	GENERATED_BODY()

	FVector Delta = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FVector Velocity = FVector::ZeroVector;

	bool bSweep = true;
	bool bShouldDetach = false;
	EDetachFromLadderMethod DetachMethod = EDetachFromLadderMethod::Fall;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API ULadderTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULadderTraversalComponent();

	void Initialize(UGCBaseCharacterMovementComponent* InMovementComponent);


	bool AttachToLadder(
		const ALadder* Ladder,
		const FVector& CurrentLocation,
		const FRotator& CurrentRotation,
		FVector& OutLocation,
		FRotator& OutRotation,
		bool& bOutAttachFromTop
	);

	void ResetAttach();
	void NotifyDetachedFromLadder();
	void ClearCurrentLadder();

	bool HasValidLadder() const;
	bool IsAttachInProgress() const;
	bool CanAttachToLadder() const;

	const ALadder* GetCurrentLadder() const { return CurrentLadder; }

	float GetClimbingSpeed() const { return ClimbingSpeed; }
	float GetJumpOffFromLadderSpeed() const { return JumpOffFromLadderSpeed; }
	float GetMinBottomOffset() const { return MinBottomOffset; }
	float GetMaxTopOffset() const { return MaxTopOffset; }
	float GetLadderToCharacterOffset() const { return LadderToCharacterOffset; }

	float GetProjection(const FVector& Location) const;
	FVector GetAlignedLocation(float LadderProjection) const;
	FRotator GetTargetRotation() const;
	float GetSpeedRatio(const FVector& Velocity) const;

	bool BuildAttachMovement(
		float DeltaTime,
		const FVector& CurrentLocation,
		const FQuat& CurrentRotation,
		FLadderTraversalMovementFrame& OutFrame,
		bool& bOutFinished
	);

	bool BuildClimbMovement(
		float DeltaTime,
		const FVector& CurrentLocation,
		const FRotator& CurrentRotation,
		float Input,
		bool bInputBlocked,
		FLadderTraversalMovementFrame& OutFrame
	);

private:
	void StartAttachFromTop(const FVector& CurrentLocation, const FRotator& CurrentRotation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder|Attach", meta = (AllowPrivateAccess = "true"))
	FLadderAttachMovementSettings AttachSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float ReattachCooldown = 0.4f;

	float LastDetachTime = -1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingBreakingDeceleration = 2048.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float MinBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float MaxTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder", meta = (AllowPrivateAccess = "true", ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(Transient)
	TObjectPtr<UGCBaseCharacterMovementComponent> MovementComponent = nullptr;

	const ALadder* CurrentLadder = nullptr;

	FLadderAttachMovementState AttachState;
};
