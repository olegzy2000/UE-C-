// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LedgeDetectorComponent.h"
#include "../../Actors/Interactive/Environment/Ladder.h"
#include "../../Actors/Interactive/Environment/Zipline.h"
#include "RunWallDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../Platformes/BasePlatform.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCBaseCharacterMovementComponent.generated.h"
struct FMantlingMovementParameters {
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	FVector InitialAnimationLocation = FVector::ZeroVector;
	float Duration = 1.0f;
	float StartTime = 0.0f;
	UCurveVector* MantlingCurve;
	UPROPERTY()
	ABasePlatform* HitObject;

};
UENUM(BlueprintType)
enum class ECustomMovementMode :uint8 {
	CMOVE_None=0 UMETA(DisplayName="None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Slide UMETA(DisplayName = "Slide"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_RunWall UMETA(DisplayName = "RunWall"),
	CMove_Max UMETA(hidden)
};
UENUM(BlueprintType)
enum class EDetachFromLadderMethod :uint8 {
	Fall=0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff

};
UENUM(BlueprintType)
enum class EDetachFromRunWallMethod :uint8 {
	Fall = 0,
	JumpOff
};
/**
 * 
 */
class AGCBaseCharacter;
UCLASS()
class MYPROJECT_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	FORCEINLINE bool IsSprinting();
	void SetIsSprinting(bool flag);
	virtual void UpdateFromCompressedFlags(uint8 Flags);
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	UGCBaseCharacterMovementComponent();
	FORCEINLINE bool IsProning();
	FORCEINLINE bool IsCrouched();
	bool IsSlide();
	virtual float GetMaxSpeed() const override;
	bool GetIsLeft();
	void StartSprint();
	void StopSprint();
	void ChangeProneState();
	void ChangeCrouchState();
	void TryToRunWall();
	void TryToSlide();
	void SlideStart();
	void SlideStop();
	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	void StartRunOnWall();
	bool IsMantling();
	bool IsRunningOnWall();
	void EndRunningOnWall(EDetachFromRunWallMethod DetachType);
	void AttachToLadder(const ALadder* Ladder);
	void AttachToZipline( AZipline* Zipline);
	float GetActorToCurrentLadderProjection(const FVector& Location);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod);
	void DetachFromZipline(EDetachFromLadderMethod DetachFromLadderMethod);
	bool IsOnLadder()const;
	bool IsOnZipline() const;
	const ALadder* GetCurrentLadder();
	const AZipline* GetCurrentZipline();
	virtual void PhysicsRotation(float DeltaTime) override;
	float GetLadderSpeedRation() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float SprintSpeed = 1200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float SwimmingCapsuleRadius = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float SwimmingCapsuleHalfSize = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float ClimbingOnLadderMaxSpeed = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float ClimbingOnLadderBreakingDecelaration = 2048.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxLadderTopOffset = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinLadderBottomOffset = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float JumpOffFromLadderSpeed = 500.0f;
	AGCBaseCharacter* GetBaseCharacterOwner() const;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Zipline")
	float ZiplineSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: RunWall", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float JumpOffFromRunWall = 500.0f;
	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SlideSpeed = 1100.0f;
	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SlideCaspsuleHalfHeight = 60.0f;
	UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SlideMaxTime = 2.0f;
	float ZiplineOffset = 110.0f;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysRunWall(float DeltaTime, int32 Iterations);
	void PhysSlide(float DeltaTime, int32 Iterations);
	void PhysZipline(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);
private:
	bool bIsSprinting=false;
	bool bIsProning=false;
	bool bIsCrouched = false;
	bool bIsRunningOnWall=false;
	URunWallDetectorComponent* RunWallDetectorComponent;
	FRunWallDescription RunWallDescription;
	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;
	FTimerHandle RunWallTimer;
	const ALadder* CurrentLadder = nullptr;
	AZipline* CurrentZipline = nullptr;
	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
};
class FSavedMove_GC : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;
private:
	uint8 bSavedIsSprinting : 1;
public:
	virtual void Clear() override;
	virtual uint8 GetCompressedFalgs() const;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter * Character,float InDeltaTimeFVector,FVector const& NewAccel,class FNetworkPredictionData_Client_Character & ClientDataCharacter) override;
	virtual void PrepMoveFor(ACharacter* Character);
};

class FNetworkPredictionData_Client_Character_GC : public FNetworkPredictionData_Client_Character {
	typedef FNetworkPredictionData_Client_Character Super;
public:
	FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};