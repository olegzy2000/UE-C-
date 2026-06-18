#include "CameraBehaviorComponent.h"
#include "MyProject.h"
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
			UE_LOG(LogCharacter, Error, TEXT("UCameraBehaviorComponent can only be attached to APlayerCharacter!"));
		}
	}
}

void UCameraBehaviorComponent::InitializeCurves()
{
	CreateDefaultCurves();

	UCurveFloat* SprintCurve = GetSprintCurve();
	if (SprintCurve)
	{
		FOnTimelineFloat Callback;
		Callback.BindUFunction(this, FName("OnSpringArmLengthUpdate"));
		SpringArmTimeline.AddInterpFloat(SprintCurve, Callback);
		SpringArmTimeline.SetLooping(false);

		UE_LOG(LogCharacter, Log, TEXT("Sprint camera curve initialized: %s"),
			SprintCurve == DefaultSprintCurve ? TEXT("Default") : TEXT("From Blueprint"));
	}

	UCurveFloat* FOVCurve = GetFOVCurve();
	if (FOVCurve)
	{
		FOnTimelineFloat Callback;
		Callback.BindUFunction(this, FName("OnFOVUpdate"));
		FOVTimeline.AddInterpFloat(FOVCurve, Callback);
		FOVTimeline.SetLooping(false);

		UE_LOG(LogCharacter, Log, TEXT("Aim FOV curve initialized: %s"),
			FOVCurve == DefaultFOVCurve ? TEXT("Default") : TEXT("From Blueprint"));
	}
}

void UCameraBehaviorComponent::CreateDefaultCurves()
{
	DefaultSprintCurve = NewObject<UCurveFloat>(this, TEXT("DefaultSprintCurve"));
	if (DefaultSprintCurve)
	{
		FKeyHandle Handle0 = DefaultSprintCurve->FloatCurve.AddKey(0.0f, 0.0f);
		FKeyHandle Handle1 = DefaultSprintCurve->FloatCurve.AddKey(DefaultSpringArmDuration, 1.0f);
		DefaultSprintCurve->FloatCurve.SetKeyInterpMode(Handle0, RCIM_Linear);
		DefaultSprintCurve->FloatCurve.SetKeyInterpMode(Handle1, RCIM_Linear);
	}

	DefaultFOVCurve = NewObject<UCurveFloat>(this, TEXT("DefaultFOVCurve"));
	if (DefaultFOVCurve)
	{
		FKeyHandle Handle0 = DefaultFOVCurve->FloatCurve.AddKey(0.0f, 0.0f);
		FKeyHandle Handle1 = DefaultFOVCurve->FloatCurve.AddKey(TimeToAim, 1.0f);
		DefaultFOVCurve->FloatCurve.SetKeyInterpMode(Handle0, RCIM_Linear);
		DefaultFOVCurve->FloatCurve.SetKeyInterpMode(Handle1, RCIM_Linear);
	}
}

UCurveFloat* UCameraBehaviorComponent::GetSprintCurve() const
{
	if (SprintCameraCurve && IsValid(SprintCameraCurve))
	{
		return SprintCameraCurve;
	}
	return DefaultSprintCurve;
}

UCurveFloat* UCameraBehaviorComponent::GetFOVCurve() const
{
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
	if (ShoulderTimeline.IsPlaying())
	{
		ShoulderTimeline.TickTimeline(DeltaTime);
	}
}

void UCameraBehaviorComponent::Initialize(USpringArmComponent* InSpringArm, UCameraComponent* InCamera)
{
	CachedSpringArm = InSpringArm;
	CachedCamera = InCamera;

	if (CachedSpringArm)
	{
		OriginalSpringArmLength = CachedSpringArm->TargetArmLength;
		StartSpringArmLength = OriginalSpringArmLength;
		TargetSpringArmLength = OriginalSpringArmLength;
	}

	if (CachedCamera)
	{
		BaseFOV = CachedCamera->FieldOfView;
		StartFOV = BaseFOV;
		TargetFOV = BaseFOV;
		CurrentFOV = BaseFOV;


		OriginalCameraOffset = CachedCamera->GetRelativeLocation();


		if (OriginalCameraOffset.Y < 0)
		{
			bIsRightShoulder = false;
			OriginalCameraOffset.Y = -OriginalCameraOffset.Y; 
		}
		else
		{
			bIsRightShoulder = true;
		}

		StartShoulderOffset = CachedCamera->GetRelativeLocation();
		TargetShoulderOffset = StartShoulderOffset;
		CurrentShoulderOffset = StartShoulderOffset;
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
void UCameraBehaviorComponent::SwitchShoulderPosition()
{
	if (!CachedCamera) return;

	
	StartShoulderOffset = CachedCamera->GetRelativeLocation();

	float AbsoluteY = FMath::Abs(StartShoulderOffset.Y);

	if (bIsRightShoulder)
	{
		TargetShoulderOffset = FVector(OriginalCameraOffset.X, -AbsoluteY, OriginalCameraOffset.Z);
		bIsRightShoulder = false;
	}
	else
	{
		TargetShoulderOffset = FVector(OriginalCameraOffset.X, AbsoluteY, OriginalCameraOffset.Z);
		bIsRightShoulder = true;
	}

	if (!ShoulderTimeline.IsPlaying())
	{
		UCurveFloat* LinearCurve = NewObject<UCurveFloat>(this);
		if (LinearCurve)
		{
			LinearCurve->FloatCurve.AddKey(0.0f, 0.0f);
			LinearCurve->FloatCurve.AddKey(ShoulderSwitchDuration, 1.0f);

			FOnTimelineFloat Callback;
			Callback.BindUFunction(this, FName("OnShoulderPositionUpdate"));
			ShoulderTimeline.AddInterpFloat(LinearCurve, Callback);
			ShoulderTimeline.SetLooping(false);
		}
	}
	else
	{
		ShoulderTimeline.Stop();
	}

	ShoulderTimeline.PlayFromStart();

	UE_LOG(LogCharacter, Log, TEXT("Switching shoulder position to: %s (Y: %.1f -> %.1f)"),
		bIsRightShoulder ? TEXT("Right") : TEXT("Left"),
		StartShoulderOffset.Y, TargetShoulderOffset.Y);
}


void UCameraBehaviorComponent::StartSprintCameraTransition(float SprintLength)
{
	if (!CachedSpringArm) 
		return;

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
	if (!CachedSpringArm) 
		return;

	StartSpringArmLength = CachedSpringArm->TargetArmLength;

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

	if (!IsSprintTransitioning() && CachedSpringArm)
	{
		CachedSpringArm->TargetArmLength = OriginalSpringArmLength;
		StartSpringArmLength = OriginalSpringArmLength;
		TargetSpringArmLength = OriginalSpringArmLength;
	}

	UE_LOG(LogCharacter, Log, TEXT("Default SpringArm length updated to: %.1f"), OriginalSpringArmLength);
}

void UCameraBehaviorComponent::StartAimFOVTransition(float InTargetFOV)
{
	if (!CachedCamera) return;

	StartFOV = CachedCamera->FieldOfView;
	TargetFOV = InTargetFOV;


	if (FOVTimeline.IsPlaying())
	{
		FOVTimeline.Stop();
	}

	FOVTimeline.SetPlayRate(1.0f);
	FOVTimeline.PlayFromStart();

	UE_LOG(LogCharacter, Verbose, TEXT("Start Aim FOV transition: %.1f -> %.1f over %.2f seconds"),
		StartFOV, TargetFOV, TimeToAim);
}

void UCameraBehaviorComponent::StopAimFOVTransition()
{
	if (!CachedCamera) 
		return;

	StartFOV = CachedCamera->FieldOfView;
	TargetFOV = BaseFOV;

	if (FMath::IsNearlyEqual(StartFOV, TargetFOV, 0.1f))
	{
		if (FOVTimeline.IsPlaying())
		{
			FOVTimeline.Stop();
		}
		return;
	}


	if (FOVTimeline.IsPlaying())
	{
		FOVTimeline.Stop();
	}

	FOVTimeline.PlayFromStart();

	UE_LOG(LogCharacter, Verbose, TEXT("Stop Aim FOV transition: %.1f -> %.1f over %.2f seconds"),
		StartFOV, TargetFOV, TimeToAim);
}

void UCameraBehaviorComponent::UpdateBaseFOV(float NewBaseFOV)
{
	BaseFOV = NewBaseFOV;

	if (!bIsAiming && CachedCamera)
	{
		CachedCamera->SetFieldOfView(BaseFOV);
		StartFOV = BaseFOV;
		TargetFOV = BaseFOV;
		CurrentFOV = BaseFOV;

		if (FOVTimeline.IsPlaying())
		{
			FOVTimeline.Stop();
		}
	}

	UE_LOG(LogCharacter, Log, TEXT("Base FOV updated to: %.1f"), BaseFOV);
}

float UCameraBehaviorComponent::getDefaultPositionOfCamera()
{
	return DefaultPositionOfCamera;
}

void UCameraBehaviorComponent::OnShoulderPositionUpdate(float Alpha)
{
	if (!CachedCamera) return;

	FVector NewPosition = FMath::Lerp(StartShoulderOffset, TargetShoulderOffset, Alpha);
	CachedCamera->SetRelativeLocation(NewPosition);
	CurrentShoulderOffset = NewPosition;
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

	float NewFOV = FMath::Lerp(StartFOV, TargetFOV, Alpha);
	ApplyFOV(NewFOV);
}

void UCameraBehaviorComponent::ApplyFOV(float NewFOV)
{
	if (!CachedCamera) return;

	CachedCamera->SetFieldOfView(NewFOV);

	if (FMath::Abs(NewFOV - CurrentFOV) > 1.0f)
	{
		CurrentFOV = NewFOV;
		UE_LOG(LogCharacter, Verbose, TEXT("FOV updated: %.1f"), CurrentFOV);
	}
}