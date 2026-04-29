// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TimerManager.h"
#include "../GameMode/PayerGameModeBaseSecondVersion.h"
#include "Controllers/GCPlayerController.h"
#include "GameFramework/PhysicsVolume.h"
#include "Engine/DamageEvents.h"
#include "Components/WidgetComponent.h"
#include "../Widget/ProgressBarWidget.h"
#include "Components/TimelineComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GCBaseCharacter.h"
#include "CoreMinimal.h"
#include "../Components/PlayerComponents/CameraBehaviorComponent.h"
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

	// Движение
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	// Состояния персонажа
	virtual void ChangeProneState() override;
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

	// Стрельба и прицеливание
	virtual void StartAiming() override;
	virtual void StopAiming() override;
	virtual void StartFire() override;
	virtual void StopFire() override;

	// Плавание
	virtual void SwimForward(float Value) override;
	virtual void SwimUp(float Value) override;
	virtual void SwimRight(float Value) override;

	// Камера
	void SwitchCameraPosition();

	// Урон и здоровье
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void UpdateHealthBar();

	// Стамина
	UFUNCTION()
	void RestoreStaminaProgressBar();

	// Геттеры
	class UCameraBehaviorComponent* GetCameraBehaviorComponent() const { return CameraBehaviorComponent; }

protected:
	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

private:
	// Компоненты
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraBehaviorComponent* CameraBehaviorComponent;

	// Кешированные ссылки
	UPROPERTY()
	AGCPlayerController* PlayerController;

	// Параметры камеры
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera", meta = (AllowPrivateAccess = "true"))
	bool bIsCameraOnRightPosition = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultPositionOfCamera = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera", meta = (AllowPrivateAccess = "true"))
	float TimeToAim = 1.f;

	// Параметры движения
	UPROPERTY(EditInstanceOnly, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float SpringArmLengthInSprint = 140.0f;

	UPROPERTY(EditInstanceOnly, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float DefaultSpringArmLength = 350.0f;

	// Параметры стамины
	UPROPERTY()
	float TimeStamina;

	// Параметры кислорода
	UPROPERTY()
	float TimeOxygen;

	UPROPERTY()
	bool IsStartUseOxygen = false;

	// Таймеры
	UPROPERTY()
	FTimerHandle StopAimTimerHandle;

	// Состояния стрельбы
	bool bIsCallingAimingByFire = false;

	// Вспомогательные методы
	void TickOxygen(float DeltaTime);
	void ChangeColorOfProgressBar();
	void ChangeSpeedParamAfterFatigue();

	// Инициализация
	void InitStaminaParameters();
	void InitHealthParameters();
	void InitOxygenParameters();
	void InitTimelineToOxygenProgressBar();
	void InitTimelineCurveToOxygenProgressBar();

	// Oxygen Timeline (оставляем пока здесь, потом вынесем в OxygenManagerComponent)
	UPROPERTY()
	FTimeline TimelineForOxygenProgressBar;

	UPROPERTY()
	UCurveFloat* TimelineCurveForOxygenProgressBar;

	// Колбэки для Oxygen
	UFUNCTION()
	void OxygenProgressBarUpdate(float alpha);

	void StartProgressBarOxygenPercent();
	void ReverseProgressBarOxygenPercent();

	// Sprint Timeline (скоро будет вынесен)
	UPROPERTY()
	FTimeline TimelineForStaminaProgressBar;

	UPROPERTY()
	UCurveFloat* TimelineCurveForStaminaProgressBar;

	void InitTimelineToStaminaProgressBar();
	void InitTimelineCurveToStaminaProgressBar();

	UFUNCTION()
	void StaminaProgressBarUpdate(float alpha);

	void StartResizeProgressBarPercent();
	void ReverseResizeProgressBarPercent();

protected:
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* StaminaWidgetComponent;
};