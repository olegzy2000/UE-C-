// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterFootIKComponent.h"
#include "MyProject.h"
#include "Engine/GameInstance.h"

#include "Characters/GCBaseCharacter.h"
#include "Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "../../Characters/Animations/GCBaseCharacterAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/DebugSubsystem.h"
#include "GameCodeTypes.h"

UCharacterFootIKComponent::UCharacterFootIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterFootIKComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerDependencies();
	CacheFootBoneRelativeLocations();
	EnsureDefaultTimelineCurve();
	InitTimelines();
}

void UCharacterFootIKComponent::CacheOwnerDependencies()
{
	CachedBaseCharacter = Cast<AGCBaseCharacter>(GetOwner());
	if (!CachedBaseCharacter.IsValid()) {
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterFootIKComponent::CacheOwnerDependencies failed: owner is not AGCBaseCharacter"));
		return;
	}

	CachedBaseCharacterMovementComponent = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	if (!CachedBaseCharacterMovementComponent.IsValid()) {
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterFootIKComponent::CacheOwnerDependencies failed: movement component is not valid"));
	}
}

void UCharacterFootIKComponent::CacheFootBoneRelativeLocations()
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CachedBaseCharacter->GetMesh();
	if (!IsValid(CharacterMesh) || !IsValid(CharacterMesh->GetSkeletalMeshAsset())) {
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterFootIKComponent::CacheFootBoneRelativeLocations failed: character mesh is not valid"));
		return;
	}

	InitialMeshRelativeLocation = CharacterMesh->GetRelativeLocation();

	const FVector LeftFootBoneWorldLocation = CharacterMesh->GetBoneLocation(LeftFootBoneName);
	LeftFootBoneRelativeLocation = CachedBaseCharacter->GetActorTransform().InverseTransformPosition(LeftFootBoneWorldLocation);

	const FVector RightFootBoneWorldLocation = CharacterMesh->GetBoneLocation(RightFootBoneName);
	RightFootBoneRelativeLocation = CachedBaseCharacter->GetActorTransform().InverseTransformPosition(RightFootBoneWorldLocation);
}

void UCharacterFootIKComponent::EnsureDefaultTimelineCurve()
{
	if (IsValid(TimelineCurveForIKFoot)) {
		return;
	}

	TimelineCurveForIKFoot = NewObject<UCurveFloat>(this, UCurveFloat::StaticClass(), TEXT("DefaultFootIKTimelineCurve"));
	if (!IsValid(TimelineCurveForIKFoot)) {
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterFootIKComponent::EnsureDefaultTimelineCurve failed to create default curve"));
		return;
	}

	FRichCurve& FloatCurve = TimelineCurveForIKFoot->FloatCurve;
	FloatCurve.Reset();
	FloatCurve.AddKey(0.0f, 0.0f);
	FloatCurve.AddKey(1.0f, 1.0f);

	UE_LOG(LogCharacter, Display, TEXT("UCharacterFootIKComponent: Blueprint IK curve is missing, using default linear curve."));
}

void UCharacterFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableFootIK) {
		return;
	}

	TimelineForIkFoot.TickTimeline(DeltaTime);
	TimelineForSkeletonPosition.TickTimeline(DeltaTime);
	CalculateFootIKPosition();

#if ENABLE_DRAW_DEBUG
	RefreshDebugSettings();
#endif
}

void UCharacterFootIKComponent::RefreshDebugSettings()
{
	if (!CachedBaseCharacter.IsValid() || !CachedBaseCharacter->GetWorld()) {
		bIsDebugIkCalculationEnable = false;
		return;
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(CachedBaseCharacter->GetWorld());
	if (!IsValid(GameInstance)) {
		bIsDebugIkCalculationEnable = false;
		return;
	}

	UDebugSubsystem* DebugSubsystem = GameInstance->GetSubsystem<UDebugSubsystem>();
	bIsDebugIkCalculationEnable = IsValid(DebugSubsystem) && DebugSubsystem->IsCategoryEnable(DebugCategoryIkCalculation);
}

void UCharacterFootIKComponent::CalculateFootIKPosition()
{
	if (!CachedBaseCharacter.IsValid() || !CachedBaseCharacterMovementComponent.IsValid()) {
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CachedBaseCharacter->GetMesh();
	if (!IsValid(CharacterMesh)) {
		return;
	}

	UGCBaseCharacterAnimInstance* GCBaseCharacterAnimInstance = Cast<UGCBaseCharacterAnimInstance>(CharacterMesh->GetAnimInstance());
	if (!IsValid(GCBaseCharacterAnimInstance)) {
		return;
	}

	if (CachedBaseCharacter->GetVelocity().Size2D() < StationaryVelocityTolerance && !CachedBaseCharacterMovementComponent->IsMantling()) {
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(CachedBaseCharacter.Get());

		FHitResult LeftTraceHit;
		const FVector LeftFootLocation = CachedBaseCharacter->GetTransform().TransformPosition(LeftFootBoneRelativeLocation);
		const EDrawDebugTrace::Type DebugTrace = bIsDebugIkCalculationEnable ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
		const bool bLeftFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(
			CachedBaseCharacter.Get(),
			LeftFootLocation + FVector(0.f, 0.0f, FootTraceUpOffset),
			LeftFootLocation + FVector(0.f, 0.f, -FootTraceDownOffset),
			FootTraceRadius,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ActorsToIgnore,
			DebugTrace,
			LeftTraceHit,
			true
		);

		FHitResult RightTraceHit;
		const FVector RightFootLocation = CachedBaseCharacter->GetTransform().TransformPosition(RightFootBoneRelativeLocation);
		const bool bRightFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(
			CachedBaseCharacter.Get(),
			RightFootLocation + FVector(0.f, 0.0f, FootTraceUpOffset),
			RightFootLocation + FVector(0.f, 0.f, -FootTraceDownOffset),
			FootTraceRadius,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ActorsToIgnore,
			DebugTrace,
			RightTraceHit,
			true
		);

		if (!bLeftFootTraceHit || !bRightFootTraceHit) {
			return;
		}

		float ZOffset = 0;
		if (RightTraceHit.ImpactPoint.Z > 0 && LeftTraceHit.ImpactPoint.Z > 0 && FMath::Abs(RightTraceHit.ImpactPoint.Z - LeftTraceHit.ImpactPoint.Z) < MaxFootHeightDifference) {
			if (LeftTraceHit.ImpactPoint.Z < RightTraceHit.ImpactPoint.Z) {
				ZOffset = LeftFootLocation.Z - LeftTraceHit.ImpactPoint.Z;
				StartEffectorPosition = RightFootBoneRelativeLocation;
				FinalEffectorPosition = RightTraceHit.ImpactPoint + FVector(0.f, 0.f, FootGroundOffset);
				bChangeRightEffector = true;
				bChangeLeftEffector = false;
				GCBaseCharacterAnimInstance->SetLeftFootAlpha(0.f);
				GCBaseCharacterAnimInstance->SetRightFootAlpha(1.f);
				TimelineForIkFoot.Play();
			}
			else {
				ZOffset = RightFootLocation.Z - RightTraceHit.ImpactPoint.Z;
				bChangeRightEffector = false;
				bChangeLeftEffector = true;
				StartEffectorPosition = LeftFootBoneRelativeLocation;
				FinalEffectorPosition = LeftTraceHit.ImpactPoint + FVector(0.f, 0.f, FootGroundOffset);
				GCBaseCharacterAnimInstance->SetRightFootAlpha(0.f);
				GCBaseCharacterAnimInstance->SetLeftFootAlpha(1.f);
				if (FMath::IsNearlyEqual(RightTraceHit.ImpactPoint.Z, LeftTraceHit.ImpactPoint.Z, 0.1f)) {
					GCBaseCharacterAnimInstance->setLeftEffectorLocation(FinalEffectorPosition);
				}
				else {
					TimelineForIkFoot.Play();
				}
			}

			StartSkeletonPosition = CharacterMesh->GetRelativeLocation();
			if (CachedBaseCharacterMovementComponent->IsCrouched()) {
				ZOffset = -18.0f;
			}

			EndSkeletonPosition = InitialMeshRelativeLocation + FVector(0.f, 0.f, -ZOffset + FootGroundOffset);
			if (CachedBaseCharacterMovementComponent->IsCrouched()) {
				ChangeSkeletalMeshPosition(InitialMeshRelativeLocation + FVector(0.f, 0.f, -ZOffset + FootGroundOffset));
			}
			else if (FMath::Abs(LeftTraceHit.ImpactPoint.Z - RightTraceHit.ImpactPoint.Z) > 1.0f && CachedBaseCharacterMovementComponent->IsProning()) {
				ChangeSkeletalMeshPosition(InitialMeshRelativeLocation + FVector(0.f, 0.f, -ZOffset + 60.0f));
			}
			else {
				TimelineForSkeletonPosition.PlayFromStart();
			}
		}
	}
	else if (!CachedBaseCharacterMovementComponent->IsProning() && !CachedBaseCharacterMovementComponent->IsCrouched()) {
		StartSkeletonPosition = CharacterMesh->GetRelativeLocation();
		EndSkeletonPosition = InitialMeshRelativeLocation;
		if (TimelineForSkeletonPosition.IsPlaying()) {
			TimelineForSkeletonPosition.Stop();
		}
		if (TimelineForIkFoot.IsPlaying()) {
			TimelineForIkFoot.Stop();
		}
		GCBaseCharacterAnimInstance->SetLeftFootAlpha(0.f);
		GCBaseCharacterAnimInstance->SetRightFootAlpha(0.f);
		ChangeSkeletalMeshPosition(InitialMeshRelativeLocation);
	}
}

void UCharacterFootIKComponent::InitTimelines()
{
	EnsureDefaultTimelineCurve();

	if (!IsValid(TimelineCurveForIKFoot)) {
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterFootIKComponent::InitTimelines failed: TimelineCurveForIKFoot is not valid"));
		return;
	}

	TimelineForIkFoot = FTimeline();
	TimelineForSkeletonPosition = FTimeline();

	FOnTimelineFloatStatic IKFootTimelineUpdate;
	IKFootTimelineUpdate.BindUObject(this, &UCharacterFootIKComponent::IKFootPositionUpdate);
	TimelineForIkFoot.AddInterpFloat(TimelineCurveForIKFoot, IKFootTimelineUpdate);
	TimelineForIkFoot.SetTimelineLength(TimelineLength);
	TimelineForIkFoot.SetLooping(false);

	FOnTimelineFloatStatic IKSkeletonTimelineUpdate;
	IKSkeletonTimelineUpdate.BindUObject(this, &UCharacterFootIKComponent::IKSkeletonPositionUpdate);
	TimelineForSkeletonPosition.AddInterpFloat(TimelineCurveForIKFoot, IKSkeletonTimelineUpdate);
	TimelineForSkeletonPosition.SetTimelineLength(TimelineLength);
	TimelineForSkeletonPosition.SetLooping(false);
}

void UCharacterFootIKComponent::IKFootPositionUpdate(float Alpha)
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CachedBaseCharacter->GetMesh();
	if (!IsValid(CharacterMesh)) {
		return;
	}

	UGCBaseCharacterAnimInstance* AnimInstance = Cast<UGCBaseCharacterAnimInstance>(CharacterMesh->GetAnimInstance());
	if (!IsValid(AnimInstance)) {
		return;
	}

	const FVector CurrentEffectorPosition = FMath::Lerp(StartEffectorPosition, FinalEffectorPosition, Alpha);
	if (bChangeRightEffector) {
		AnimInstance->setRightEffectorLocation(CurrentEffectorPosition);
	}
	else if (bChangeLeftEffector) {
		AnimInstance->setLeftEffectorLocation(CurrentEffectorPosition);
	}

	if (CurrentEffectorPosition.Equals(FinalEffectorPosition, 0.1f)) {
		TimelineForIkFoot.Stop();
	}
}

void UCharacterFootIKComponent::IKSkeletonPositionUpdate(float Alpha)
{
	const FVector CurrentEffectorPosition = FMath::Lerp(StartSkeletonPosition, EndSkeletonPosition, Alpha);
	if (CurrentEffectorPosition.Equals(EndSkeletonPosition, 0.1f)) {
		TimelineForSkeletonPosition.Stop();
	}

	ChangeSkeletalMeshPosition(CurrentEffectorPosition);
}

void UCharacterFootIKComponent::ChangeSkeletalMeshPosition(const FVector& Position)
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CachedBaseCharacter->GetMesh();
	if (IsValid(CharacterMesh)) {
		CharacterMesh->SetRelativeLocation(Position);
	}
}
