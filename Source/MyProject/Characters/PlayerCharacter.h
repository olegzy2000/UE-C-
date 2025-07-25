// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TimerManager.h"
#include "../GameMode/PayerGameModeBaseSecondVersion.h"
#include "Controllers/GCPlayerController.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/WidgetComponent.h"
#include "../Widget/ProgressBarWidget.h"
#include "Components/TimelineComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GCBaseCharacter.h"
#include "CoreMinimal.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API APlayerCharacter : public AGCBaseCharacter
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	void InitDefaultCameraBehavior();
	void InitAimCameraBehavior();
	void InitSwimmingCameraBehavior();
	virtual void Tick(float DeltaTime) override;
	void TickOxygen(float DeltaTime);
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;
	virtual void ChangeProneState() override;
	virtual void SwitchCameraPosition() override;
	virtual void SwimForward(float Value) override;
	virtual void SwimUp(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnSprintEnd_Implementation() override;
	virtual void OnSprintStart_Implementation() override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;
	virtual void StartSprint() override;
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void StopSprint() override;
	virtual void Jump() override;
	void Slide() override;
	void UpdateHealthBar();
	UFUNCTION()
	void RestoreStaminaProgressBar();
	virtual void StartAiming() override;
	virtual void StopAiming() override;
	virtual void StartFire() override;
	virtual void StopFire() override;
private:
	float TimeStamina;
	float TimeOxygen;
	float DefaultFOV;
	float CurrentFOV;
	bool IsStartUseOxygen=false;
	void InitStaminaParameters();
	void InitHealthParameters();
	void InitOxygenParameters();
	void InitTimelineToOxygenProgressBar();
	void InitTimelineToAimCamera();
	void InitTimelineCurveToOxygenProgressBar();
	void StaminaProgressBarUpdate(float alpha);
	void OxygenProgressBarUpdate(float alpha);
	void InitTimelineToSprintCamera();
	void ChangeSpeedParamAfterFatigue();
	void ChangeColorOfProgressBar();
	void InitTimelineToStaminaProgressBar();
	void InitTimelineCurveToSprintCamera();
	void InitTimelineCurveToStaminaProgressBar();
	void FovToAimUpdate(float Alpha);
	void SpringArmTargetLengthUpdate(float Alpha);
	void StartResizeSpringArmLength();
	void ReverseResizeSpringArmLength();
	void StartResizeProgressBarPercent();
	void ReverseResizeProgressBarPercent();
	void StartProgressBarOxygenPercent();
	void ReverseProgressBarOxygenPercent();
	void InitTimelineCurveForAimCamera();
	UPROPERTY()
	AGCPlayerController* PlayerController;
	UPROPERTY()
	FTimerHandle StopAimTimerHandle;
	UPROPERTY()
	FTimeline TimelineForStaminaProgressBar;
	UCurveFloat* TimelineCurveForStaminaProgressBar;
	UPROPERTY()
	FTimeline TimelineForOxygenProgressBar;
	UCurveFloat* TimelineCurveForOxygenProgressBar;
	UPROPERTY()
	FTimeline TimelineForAimCamera;
	UCurveFloat* TimelineCurveForAimCamera;
	bool bIsCallingAimingByFireFunction = false;
protected:
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* StaminaWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
		bool bIsCameraOnRightPosition = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
		float DefaultPositionOfCamera = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
		float TimeToAim = 1.0f;
	UPROPERTY(EditInstanceOnly)
		float SpringArmLenghtInSprint = 100.0f;
	UPROPERTY(EditInstanceOnly)
		float DefaultSpringArmLenght = 350.0f;
	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;
};
