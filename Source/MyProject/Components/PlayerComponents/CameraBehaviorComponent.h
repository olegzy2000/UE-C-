#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"  
#include "Camera/CameraComponent.h"           
#include "CameraBehaviorComponent.generated.h"

class APlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCameraBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize(USpringArmComponent* InSpringArm, UCameraComponent* InCamera);

	void InitDefaultBehavior();
	void InitAimBehavior();

	void SwitchShoulderPosition(float DefaultPosition, bool bIsOnRightSide);

	// ћетоды дл€ спринта
	void StartSprintCameraTransition(float SprintLength);
	void StopSprintCameraTransition();

	void UpdateDefaultSpringArmLength(float NewDefaultLength);

	// ћетоды дл€ FOV интерпол€ции
	void StartAimFOVTransition(float TargetFOV, float Duration);
	void StopAimFOVTransition(float Duration);

	void UpdateBaseFOV(float NewBaseFOV);

	bool IsAiming() const { return bIsAiming; }
	bool IsFOVTransitioning() const { return FOVTimeline.IsPlaying(); }
	bool IsSprintTransitioning() const { return SpringArmTimeline.IsPlaying(); }
	float GetCurrentFOV() const { return CurrentFOV; }
	float GetBaseFOV() const { return BaseFOV; }
	float GetCurrentSpringArmLength() const { return CachedSpringArm ? CachedSpringArm->TargetArmLength : 0.0f; }

private:
	UFUNCTION()
	void OnSpringArmLengthUpdate(float Alpha);

	UFUNCTION()
	void OnFOVUpdate(float Alpha);

	//  ешированные ссылки
	UPROPERTY()
	APlayerCharacter* CachedOwner = nullptr;

	UPROPERTY()
	USpringArmComponent* CachedSpringArm = nullptr;

	UPROPERTY()
	UCameraComponent* CachedCamera = nullptr;

	// Timeline
	FTimeline SpringArmTimeline;
	FTimeline FOVTimeline;

	// ѕараметры интерпол€ции спринта
	float OriginalSpringArmLength = 350.0f;  // ќ–»√»ЌјЋ№Ќјя длина (сохран€етс€ при инициализации)
	float StartSpringArmLength = 350.0f;     // —“ј–“ќ¬јя длина дл€ текущей интерпол€ции
	float TargetSpringArmLength = 350.0f;    // ÷елева€ длина дл€ интерпол€ции
	float DefaultSpringArmDuration = 0.3f;   // ƒлительность интерпол€ции спринта

	// ѕараметры интерпол€ции FOV
	float BaseFOV = 90.0f;           // Ѕазовый FOV (без прицела)
	float StartFOV = 90.0f;          // —тартовый FOV дл€ текущей интерпол€ции
	float TargetFOV = 90.0f;         // ÷елевой FOV дл€ текущей интерпол€ции
	float CurrentFOV = 90.0f;        // “екущий FOV

	// —осто€ни€
	bool bIsAiming = false;

	//  ривые дл€ интерпол€ции (можно задать в Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SprintCameraCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* AimFOVCurve = nullptr;

	// ƒефолтные кривые (создаютс€ в коде)
	UPROPERTY()
	UCurveFloat* DefaultSprintCurve = nullptr;

	UPROPERTY()
	UCurveFloat* DefaultFOVCurve = nullptr;

	// ¬спомогательные методы
	void InitializeCurves();
	void CreateDefaultCurves();
	void CacheOwnerAndValidate();
	void ApplyFOV(float NewFOV);

	// ѕолучить кривую дл€ спринта
	UCurveFloat* GetSprintCurve() const;

	// ѕолучить кривую дл€ FOV
	UCurveFloat* GetFOVCurve() const;
};