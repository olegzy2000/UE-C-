// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterTraversalComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Components/MovementComponents/LedgeDetectorComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"

UCharacterTraversalComponent::UCharacterTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterTraversalComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedBaseCharacter = Cast<AGCBaseCharacter>(GetOwner());

	if (!CachedBaseCharacter.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCharacterTraversalComponent::BeginPlay failed: owner is not AGCBaseCharacter"));
		return;
	}

	CachedBaseCharacterMovementComponent = CachedBaseCharacter->GetBaseCharacterMovementComponent();

	if (!CachedBaseCharacterMovementComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCharacterTraversalComponent::BeginPlay failed: movement component is not valid"));
	}

	CachedLedgeDetectorComponent = CachedBaseCharacter->GetLedgeDetectorComponent();
	if (!CachedLedgeDetectorComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCharacterTraversalComponent::BeginPlay failed: ledge detector component is not valid"));
	}
}


bool UCharacterTraversalComponent::CanMantle() const
{
	
	if (!CachedBaseCharacterMovementComponent.IsValid())
	{
		return false;
	}

	UGCBaseCharacterMovementComponent* MovementComponent = CachedBaseCharacterMovementComponent.Get();
	return !MovementComponent->IsOnLadder()
		&& !MovementComponent->IsOnZipline()
		&& !MovementComponent->IsSlide();
}

void UCharacterTraversalComponent::Mantle(bool bForce)
{
	
	if (!CanMantle() && !bForce) {
		return;
	}

	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}
	AGCBaseCharacter* BaseCharacter = CachedBaseCharacter.Get();
	if (!CachedLedgeDetectorComponent.IsValid()) {
		return;
	}
	ULedgeDetectorComponent* LedgeDetectorComponent = CachedLedgeDetectorComponent.Get();
	if (!CachedBaseCharacterMovementComponent.IsValid()) {
		return;
	}
	UGCBaseCharacterMovementComponent* MovementComponent = CachedBaseCharacterMovementComponent.Get();

	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription) && !MovementComponent->IsMantling()) {
		FMantlingMovementParameters MantlingParameters;

		if (LedgeDescription.LedgeComponent != nullptr) {
			MantlingParameters.TargetComponent = LedgeDescription.LedgeComponent;
		}
		else {
			MantlingParameters.TargetComponent = nullptr;
		}

		MantlingParameters.InitialLocation = BaseCharacter->GetActorLocation();
		MantlingParameters.InitialRotation = BaseCharacter->GetActorRotation();
		MantlingParameters.TargetComponent = LedgeDescription.LedgeComponent;
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		const float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = BaseCharacter->GetMantlingSettings(MantlingHeight);

		float MinRange = 0.0f;
		float MaxRange = 0.0f;
		if (!IsValid(MantlingSettings.MantlingCurve) || !IsValid(MantlingSettings.MantlingMontage)) {
			return;
		}

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		const FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		const FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.Duration -= MantlingParameters.StartTime;
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation
			- MantlingSettings.AnimationCorrectionZ * FVector::UpVector
			+ MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		if (IsValid(MantlingParameters.TargetComponent))
		{
			const FTransform TargetComponentTransform =
				MantlingParameters.TargetComponent->GetComponentTransform();

			MantlingParameters.LocalTargetLocation =
				TargetComponentTransform.InverseTransformPosition(MantlingParameters.TargetLocation);

			MantlingParameters.LocalInitialAnimationLocation =
				TargetComponentTransform.InverseTransformPosition(MantlingParameters.InitialAnimationLocation);

			MantlingParameters.TargetComponentStartTransform = TargetComponentTransform;

			MantlingParameters.bHasMovingTarget = true;

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.0f,
					FColor::Green,
					TEXT("Mantle moving target initialized")
				);
			}
		}
		else
		{
			MantlingParameters.bHasMovingTarget = false;
		}


		MovementComponent->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = BaseCharacter->GetMesh() != nullptr ? BaseCharacter->GetMesh()->GetAnimInstance() : nullptr;
		if (IsValid(AnimInstance)) {
			AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		}
	}
}

void UCharacterTraversalComponent::TryToRunWall()
{
	if (!CachedBaseCharacterMovementComponent.IsValid())
	{
		return;
	}
	UGCBaseCharacterMovementComponent* BaseMovementComponent = CachedBaseCharacterMovementComponent.Get();
	BaseMovementComponent->TryToRunWall();
}
