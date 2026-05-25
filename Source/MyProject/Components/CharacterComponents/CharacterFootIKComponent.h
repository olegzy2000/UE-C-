// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CharacterFootIKComponent.generated.h"

class AGCBaseCharacter;
class UGCBaseCharacterMovementComponent;
class UCurveFloat;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterFootIKComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CalculateFootIKPosition();
	void RefreshDebugSettings();
	void ChangeSkeletalMeshPosition(const FVector& Position);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character | IK")
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character | IK")
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

private:
	void CacheOwnerDependencies();
	void CacheFootBoneRelativeLocations();
	void EnsureDefaultTimelineCurve();
	void InitTimelines();
	void IKFootPositionUpdate(float Alpha);
	void IKSkeletonPositionUpdate(float Alpha);

	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;
	TWeakObjectPtr<UGCBaseCharacterMovementComponent> CachedBaseCharacterMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	bool bEnableFootIK = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* TimelineCurveForIKFoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	FName RightFootBoneName = FName(TEXT("foot_r"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	FName LeftFootBoneName = FName(TEXT("foot_l"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float FootTraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float FootTraceUpOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float FootTraceDownOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float FootGroundOffset = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float MaxFootHeightDifference = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float StationaryVelocityTolerance = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK", meta = (AllowPrivateAccess = "true"))
	float TimelineLength = 0.05f;

	UPROPERTY()
	FTimeline TimelineForSkeletonPosition;

	UPROPERTY()
	FTimeline TimelineForIkFoot;

	FVector LeftFootBoneRelativeLocation = FVector::ZeroVector;
	FVector RightFootBoneRelativeLocation = FVector::ZeroVector;
	FVector InitialMeshRelativeLocation = FVector::ZeroVector;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	bool bChangeRightEffector = false;
	bool bChangeLeftEffector = false;
	bool bIsDebugIkCalculationEnable = false;

	FVector FinalEffectorPosition = FVector::ZeroVector;
	FVector StartEffectorPosition = FVector::ZeroVector;
	FVector StartSkeletonPosition = FVector::ZeroVector;
	FVector EndSkeletonPosition = FVector::ZeroVector;
};
