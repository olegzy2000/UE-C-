#include "CameraBehaviorComponent.h"
#include "../../Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCameraBehaviorComponent::UCameraBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerAndValidate();

	if (!CachedOwner)
	{
		SetComponentTickEnabled(false);
		return;
	}

	// Инициализируем кривые
	InitializeCurves();
}

void UCameraBehaviorComponent::CacheOwnerAndValidate()
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		CachedOwner = Cast<APlayerCharacter>(Owner);
		if (!CachedOwner)
		{
			UE_LOG(LogTemp, Error, TEXT("UCameraBehaviorComponent can only be attached to APlayerCharacter!"));
		}
	}
}

void UCameraBehaviorComponent::InitializeCurves()
{
	// Создаем дефолтные кривые (будут использоваться если не заданы в Blueprint)
	CreateDefaultCurves();

	// Настраиваем Timeline для спринта
	UCurveFloat* SprintCurve = GetSprintCurve();
	if (SprintCurve)
	{
		FOnTimelineFloat Callback;
		Callback.BindUFunction(this, FName("OnSpringArmLengthUpdate"));
		SpringArmTimeline.AddInterpFloat(SprintCurve, Callback);
		SpringArmTimeline.SetTimelineLength(DefaultSpringArmDuration);
		SpringArmTimeline.SetLooping(false);

		UE_LOG(LogTemp, Log, TEXT("Sprint camera curve initialized: %s"),
			SprintCurve == DefaultSprintCurve ? TEXT("Default") : TEXT("From Blueprint"));
	}

	// Настраиваем Timeline для FOV
	UCurveFloat* FOVCurve = GetFOVCurve();
	if (FOVCurve)
	{
		FOnTimelineFloat Callback;
		Callback.BindUFunction(this, FName("OnFOVUpdate"));
		FOVTimeline.AddInterpFloat(FOVCurve, Callback);
		FOVTimeline.SetLooping(false);

		UE_LOG(LogTemp, Log, TEXT("Aim FOV curve initialized: %s"),
			FOVCurve == DefaultFOVCurve ? TEXT("Default") : TEXT("From Blueprint"));
	}
}

void UCameraBehaviorComponent::CreateDefaultCurves()
{
	// Дефолтная линейная кривая для спринта
	DefaultSprintCurve = NewObject<UCurveFloat>(this, TEXT("DefaultSprintCurve"));
	if (DefaultSprintCurve)
	{
		FKeyHandle Handle0 = DefaultSprintCurve->FloatCurve.AddKey(0.0f, 0.0f);
		FKeyHandle Handle1 = DefaultSprintCurve->FloatCurve.AddKey(1.0f, 1.0f);
		DefaultSprintCurve->FloatCurve.SetKeyInterpMode(Handle0, RCIM_Linear);
		DefaultSprintCurve->FloatCurve.SetKeyInterpMode(Handle1, RCIM_Linear);
	}

	// Дефолтная линейная кривая для FOV
	DefaultFOVCurve = NewObject<UCurveFloat>(this, TEXT("DefaultFOVCurve"));
	if (DefaultFOVCurve)
	{
		FKeyHandle Handle0 = DefaultFOVCurve->FloatCurve.AddKey(0.0f, 0.0f);
		FKeyHandle Handle1 = DefaultFOVCurve->FloatCurve.AddKey(1.0f, 1.0f);
		DefaultFOVCurve->FloatCurve.SetKeyInterpMode(Handle0, RCIM_Linear);
		DefaultFOVCurve->FloatCurve.SetKeyInterpMode(Handle1, RCIM_Linear);
	}
}

UCurveFloat* UCameraBehaviorComponent::GetSprintCurve() const
{
	// Приоритет: кривая из Blueprint > дефолтная кривая
	if (SprintCameraCurve && IsValid(SprintCameraCurve))
	{
		return SprintCameraCurve;
	}
	return DefaultSprintCurve;
}

UCurveFloat* UCameraBehaviorComponent::GetFOVCurve() const
{
	// Приоритет: кривая из Blueprint > дефолтная кривая
	if (AimFOVCurve && IsValid(AimFOVCurve))
	{
		return AimFOVCurve;
	}
	return DefaultFOVCurve;
}

void UCameraBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArmTimeline.IsPlaying())
	{
		SpringArmTimeline.TickTimeline(DeltaTime);
	}

	if (FOVTimeline.IsPlaying())
	{
		FOVTimeline.TickTimeline(DeltaTime);
	}
}

void UCameraBehaviorComponent::Initialize(USpringArmComponent* InSpringArm, UCameraComponent* InCamera)
{
	CachedSpringArm = InSpringArm;
	CachedCamera = InCamera;

	if (CachedSpringArm)
	{
		// Сохраняем ОРИГИНАЛЬНУЮ длину (дефолтную)
		OriginalSpringArmLength = CachedSpringArm->TargetArmLength;
		StartSpringArmLength = OriginalSpringArmLength;
		TargetSpringArmLength = OriginalSpringArmLength;
	}

	if (CachedCamera)
	{
		// Сохраняем базовый FOV (без прицела)
		BaseFOV = CachedCamera->FieldOfView;
		StartFOV = BaseFOV;
		TargetFOV = BaseFOV;
		CurrentFOV = BaseFOV;
	}
}

void UCameraBehaviorComponent::InitDefaultBehavior()
{
	bIsAiming = false;

	if (!CachedOwner) return;

	CachedOwner->bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* Movement = CachedOwner->GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
	}
}

void UCameraBehaviorComponent::InitAimBehavior()
{
	bIsAiming = true;

	if (!CachedOwner) return;

	CachedOwner->bUseControllerRotationYaw = true;

	if (UCharacterMovementComponent* Movement = CachedOwner->GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = false;
	}
}

void UCameraBehaviorComponent::SwitchShoulderPosition(float DefaultPosition, bool bIsOnRightSide)
{
	if (!CachedCamera) return;

	float NewXOffset = bIsOnRightSide ? DefaultPosition : -DefaultPosition;
	CachedCamera->SetRelativeLocation(FVector(0.0f, NewXOffset, 0.0f));
}

void UCameraBehaviorComponent::StartSprintCameraTransition(float SprintLength)
{
	if (!CachedSpringArm) return;

	// Сохраняем текущую длину как СТАРТОВУЮ
	StartSpringArmLength = CachedSpringArm->TargetArmLength;
	TargetSpringArmLength = SprintLength;

	if (SpringArmTimeline.IsPlaying())
	{
		SpringArmTimeline.Stop();
	}
	SpringArmTimeline.PlayFromStart();
}

void UCameraBehaviorComponent::StopSprintCameraTransition()
{
	if (!CachedSpringArm) return;

	// Сохраняем текущую длину как СТАРТОВУЮ
	StartSpringArmLength = CachedSpringArm->TargetArmLength;

	// Возвращаемся к ОРИГИНАЛЬНОЙ длине
	TargetSpringArmLength = OriginalSpringArmLength;

	if (SpringArmTimeline.IsPlaying())
	{
		SpringArmTimeline.Stop();
	}
	SpringArmTimeline.PlayFromStart();
}

void UCameraBehaviorComponent::UpdateDefaultSpringArmLength(float NewDefaultLength)
{
	OriginalSpringArmLength = NewDefaultLength;

	// Если сейчас не в спринте, сразу применяем новую длину
	if (!IsSprintTransitioning() && CachedSpringArm)
	{
		CachedSpringArm->TargetArmLength = OriginalSpringArmLength;
		StartSpringArmLength = OriginalSpringArmLength;
		TargetSpringArmLength = OriginalSpringArmLength;
	}

	UE_LOG(LogTemp, Log, TEXT("Default SpringArm length updated to: %.1f"), OriginalSpringArmLength);
}

void UCameraBehaviorComponent::StartAimFOVTransition(float InTargetFOV, float Duration)
{
	if (!CachedCamera) return;

	// Сохраняем текущий FOV как стартовый для интерполяции
	StartFOV = CachedCamera->FieldOfView;
	TargetFOV = InTargetFOV;

	// Устанавливаем длительность интерполяции
	FOVTimeline.SetTimelineLength(Duration);

	if (FOVTimeline.IsPlaying())
	{
		FOVTimeline.Stop();
	}

	FOVTimeline.SetPlayRate(1.0f);
	FOVTimeline.PlayFromStart();

	UE_LOG(LogTemp, Verbose, TEXT("Start Aim FOV transition: %.1f -> %.1f over %.2f seconds"),
		StartFOV, TargetFOV, Duration);
}

void UCameraBehaviorComponent::StopAimFOVTransition(float Duration)
{
	if (!CachedCamera) return;

	// Возвращаемся к базовому FOV
	StartFOV = CachedCamera->FieldOfView;
	TargetFOV = BaseFOV;

	// Если уже достигли целевого FOV, не начинаем интерполяцию
	if (FMath::IsNearlyEqual(StartFOV, TargetFOV, 0.1f))
	{
		if (FOVTimeline.IsPlaying())
		{
			FOVTimeline.Stop();
		}
		return;
	}

	// Устанавливаем длительность интерполяции
	FOVTimeline.SetTimelineLength(Duration);

	if (FOVTimeline.IsPlaying())
	{
		FOVTimeline.Stop();
	}

	FOVTimeline.SetPlayRate(1.0f);
	FOVTimeline.PlayFromStart();

	UE_LOG(LogTemp, Verbose, TEXT("Stop Aim FOV transition: %.1f -> %.1f over %.2f seconds"),
		StartFOV, TargetFOV, Duration);
}

void UCameraBehaviorComponent::UpdateBaseFOV(float NewBaseFOV)
{
	BaseFOV = NewBaseFOV;

	// Если сейчас не в прицеле, сразу применяем новый FOV
	if (!bIsAiming && CachedCamera)
	{
		CachedCamera->SetFieldOfView(BaseFOV);
		StartFOV = BaseFOV;
		TargetFOV = BaseFOV;
		CurrentFOV = BaseFOV;

		// Останавливаем любую текущую интерполяцию
		if (FOVTimeline.IsPlaying())
		{
			FOVTimeline.Stop();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Base FOV updated to: %.1f"), BaseFOV);
}

void UCameraBehaviorComponent::OnSpringArmLengthUpdate(float Alpha)
{
	if (!CachedSpringArm) return;

	// Интерполируем между StartSpringArmLength и TargetSpringArmLength
	float NewLength = FMath::Lerp(StartSpringArmLength, TargetSpringArmLength, Alpha);
	CachedSpringArm->TargetArmLength = NewLength;
}

void UCameraBehaviorComponent::OnFOVUpdate(float Alpha)
{
	if (!CachedCamera) return;

	// Линейная интерполяция между стартовым и целевым FOV
	float NewFOV = FMath::Lerp(StartFOV, TargetFOV, Alpha);
	ApplyFOV(NewFOV);
}

void UCameraBehaviorComponent::ApplyFOV(float NewFOV)
{
	if (!CachedCamera) return;

	CachedCamera->SetFieldOfView(NewFOV);

	// Логируем только при значительных изменениях
	if (FMath::Abs(NewFOV - CurrentFOV) > 1.0f)
	{
		CurrentFOV = NewFOV;
		UE_LOG(LogTemp, Verbose, TEXT("FOV updated: %.1f"), CurrentFOV);
	}
}