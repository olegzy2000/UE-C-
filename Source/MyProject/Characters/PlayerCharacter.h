// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TimerManager.h"
#include "../GameMode/PayerGameModeBaseSecondVersion.h"
#include "Components/WidgetComponent.h"
#include "../Widget/FatigueBar.h"
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
	virtual void Tick(float DeltaTime) override;
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
	virtual void StopSprint() override;
	virtual void Jump() override;
	void Slide() override;
private:
	void FatigueProgressBarUpdate(float alpha);
	void InitTimelineToSprintCamera();
	void ChangeSpeedParamAfterFatigue();
	void ChangeColorOfProgressBar();
	void InitTimelineToFatigureProgressBar();
	void InitTimelineCurveToSprintCamera();
	void InitTimelineCurveToFatigureProgressBar();
	void SpringArmTargetLengthUpdate(float Alpha);
	void StartResizeSpringArmLength();
	void ReverseResizeSpringArmLength();
	void StartResizeProgressBarPercent();
	void ReverseResizeProgressBarPercent();
	const float MaxFatigue=1.f;
	const float MinFatigue=0.f;
	UPROPERTY()
	APayerGameModeBaseSecondVersion* GameMode;
	UPROPERTY()
	FTimeline TimelineForFatigueProgressBar;
	UPROPERTY(EditAnywhere, Category = "Character | Curve for progress bar timeline")
	UCurveFloat* TimelineCurveForProgressBar;
protected:
	UPROPERTY(EditAnywhere, category="Fatigue progress bar setting")
	float SpeedFatigue = 1.f;
	UPROPERTY(EditAnywhere, category = "Fatigue progress bar setting")
	float TimeFatigue = 5.f;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* FatigueWidgetComponent;
};
