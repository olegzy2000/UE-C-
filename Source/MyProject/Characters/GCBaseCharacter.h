// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../Actors/Interactive/InteractiveActor.h"
#include "../Actors/Interactive/Environment/Ladder.h"
#include "../Actors/Interactive/Environment/Zipline.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "../Components/MovementComponents/LedgeDetectorComponent.h"
#include "../Components/CharacterComponents/CharacterAttributeComponent.h"
#include "math.h"
#include "Animations/GCBaseCharacterAnimInstance.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "CoreMinimal.h"
#include "GCBaseCharacter.generated.h"
class UCharacterEquipmentComponent;
USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* MantlingMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UCurveVector* MantlingCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta=(ClampMin=0.0f,UMin=0.0f))
		float MaxHeight=200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UMin = 0.0f))
		float MinHeight = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UMin = 0.0f))
		float MaxHeightStartTime = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UMin = 0.0f))
		float MinHeightStartTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UMin = 0.0f))
		float AnimationCorrectionXY = 65.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UMin = 0.0f))
		float AnimationCorrectionZ = 200.0f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAminStateChanged,bool)

UCLASS(Abstract,NotBlueprintable)
class MYPROJECT_API AGCBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	void PreviousItem();
	void NextItem();
	void Reload() ;
	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void SwitchCameraPosition() {};
	virtual void ChangeCrouchState();
	virtual void StartSprint();
	virtual void StopSprint();
	virtual void Slide();
	virtual void StartFire();
	virtual void StopFire();
	virtual void StartAiming();
	virtual void StopAiming();
	virtual void EquipPrimaryItem();
	float GetAimingMovementSpeed() const;
	virtual void ChangeProneState();
	virtual void SwimRight(float Value) {};
	virtual void SwimForward(float Value) {};
	virtual void SwimUp(float Value) {};
	void ChangeCapsuleParamOnProneStateFromCrouch(float Radius,float Height);
	void ChangeCapsuleParamFromProneStateToCrouch(float Radius, float Height);
	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;
	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;
	float GetProneCapsuleHeight() {
		return ProneCapsuleHeight;
	}
	float GetProneCapsuleRadius() {
		return ProneCapsuleRadius;
	}
	float GetCrouchCapsuleHeight() {
		return CrouchCapsuleHeight;
	}
	float GetDefaultCapsuleRadius() {
		return DefaultCapsuleRadius;
	}
	float GetDefaultCapsuleHeight() {
		return DefaultCapsuleHeight;
	}
	bool IsAming();
	FTimerHandle getTimeHandler() {
		return FuzeTimerHandle;
	}
	virtual void Tick(float DeltaTime) override;
	void InitIkDebugDraw();
	virtual void Mantle(bool bForce);
	void TryToRunWall();
	UGCBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const;
	AGCBaseCharacter(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetIKRightFootOffset() const {
		return IKRightFootOffset;
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetIKLeftFootOffset() const {
		return IKLeftFootOffset;
	}
	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void ClimbLadderUp(float Value);
	void InteractionWithLadder();
	void InteractionWithZipline();
	const ALadder* GetAvailableLadder();
	AZipline* GetAvailableZipline();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
		void OnStartAiming();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Character")
		void OnStopAiming();
	FOnAminStateChanged OnAmingStateChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseTurnRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseLookUpRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float SprintSpeed = 800.0f;
	virtual bool CanSprint();
	UFUNCTION(BlueprintImplementableEvent,Category="Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	bool CanMantle() const;
	UPROPERTY()
	UGCBaseCharacterMovementComponent* GCBaseCharacterMovementComponent;
	bool bCanStartSrpint = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TimeToSwitchPositionCameraInSprint = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Character | Curve for camera timeline")
	UCurveFloat* TimelineCurveForCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY()
	FTimerHandle FuzeTimerHandle;
	UPROPERTY()
	FTimeline TimelineForCamera;
	UPROPERTY()
	FTimeline TimelineForSkeletonPosition;
	UPROPERTY()
	FTimeline TimelineForIkFoot;
	UPROPERTY(EditAnywhere, Category = "Character | IK setting")
	UCurveFloat* TimelineCurveForIKFoot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK settings",meta =(BlueprintProtected=true))
	FName RightFootBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK settings", meta = (BlueprintProtected = true))
	FName LeftFootBoneName;
	FVector LeftFootBoneRelativeLocation;
	FVector RightFootBoneRelativeLocation;
	FVector InitialMeshRalativeLocation;
	bool bIsSprintRequested = false;
	bool bCanProne = false;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const float ProneCapsuleRadius = 40.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const float ProneCapsuleHeight = 40.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const float DefaultCapsuleRadius = 34.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const float DefaultCapsuleHeight = 88.0f;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const float CrouchCapsuleHeight = 50.0f;
	void ChangeCapsuleParamOnProneState(float CapsuleRadius,float ProneCapsuleHalfHeight);
	void ChangeCapsuleParamOutProneState(float CapsuleRadius, float ProneCapsuleHalfHeight);
	bool CanStartFire();
	void ChangeMaxSpeedOfPlayer(float speed);
	void ChangeCapsuleParamFromCrouchedToIdleWalk();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	ULedgeDetectorComponent* LegDetectorComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HighMantleSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings SwimmingMantleSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Mantling" ,meta=(ClampMin=0.0f,UIMin=0.0f))
	float LowMantleMaxHeight = 125.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterAttributeComponent* CharacterAttributesComponent;
	virtual void OnDeath();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
		class UAnimMontage* OnDeathAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
		class UCurveFloat* FallDamageCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
		UCharacterEquipmentComponent* CharacterEquipmentComponent;
	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();
private:
	float CurrentAimingMovementSpeed;
	void ShowLoseText();
	FTimerHandle MyTimerHandle;
	void CalculateIkFootPosition();
	void ChangeCapsuleParamFromIdleWalkStateToCrouch();
	void InitTimelineToIKFoot();
	void IKFootPositionUpdate(float Alpha);
	void IKSkeletonPositionUpdate(float Alpha);
	void ChangeSkeletalMeshPosition(FVector Position);
	void EnableRagdoll();
	void TryChangeSprintState();
	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
	bool CanCrouch();
	void ChangeCapsuleParamFromProneToCrouched();
	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	bool bCanCrouch = true;
	bool bIsTrytoChangeCrouchPosition = false;
	bool bChangeRightEffector=false;
	bool bChangeLeftEffector=false;
	bool bIsDebugLkCalculationEnable = false;
	bool bIsAiming = false;
	FVector FinalEffectorPosition;
	FVector StartEffectorPosition;
	FVector StartSkeletonPosition;
	FVector EndSkeletonPosition;
	TArray<AInteractiveActor*>AvailableInteractiveActors;
	FVector CurrentFallApex;
	void restartCurrentLevel();
};
