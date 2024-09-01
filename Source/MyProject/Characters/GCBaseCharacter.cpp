// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <MyProject/Subsystems/DebugSubsystem.h>
#include <MyProject/Utils/GCTraceUtils.h>
#include <MyProject/GameCodeTypes.h>
#include "../Components/CharacterComponents/CharacterAttributeComponent.h"
#include "DrawDebugHelpers.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
void AGCBaseCharacter::ChangeCrouchState()
{
	if (!GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSwimming() && !GetBaseCharacterMovementComponent()->IsSlide() && !GetBaseCharacterMovementComponent()->IsSprinting()) {
		if (CanCrouch()) {
			GetBaseCharacterMovementComponent()->ChangeCrouchState();
			if (GetBaseCharacterMovementComponent()->IsProning()) {
				GetBaseCharacterMovementComponent()->ChangeProneState();
				ChangeCapsuleParamFromProneToCrouched();
				ChangeMaxSpeedOfPlayer(300.0f);
			}
			else {
				if (GetBaseCharacterMovementComponent()->IsCrouched()) {
					ChangeCapsuleParamFromIdleWalkStateToCrouch();
					ChangeMaxSpeedOfPlayer(300.0f);
				}
				else {
					ChangeCapsuleParamFromCrouchedToIdleWalk();
					ChangeMaxSpeedOfPlayer(600.0f);
				}
			}
		}
	}
}

void AGCBaseCharacter::StartSprint()
{
	if (!GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSlide()) {
		bIsSprintRequested = true;
		if (GetBaseCharacterMovementComponent()->IsCrouched()) {
			ChangeCrouchState();
		}
	}
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AGCBaseCharacter::Slide()
{
	GetBaseCharacterMovementComponent()->TryToSlide();
}


void AGCBaseCharacter::ChangeMaxSpeedOfPlayer(float speed)
{
	GetBaseCharacterMovementComponent()->MaxWalkSpeed = speed;
}
void AGCBaseCharacter::ChangeProneState()
{
	if (!GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSwimming()) {
		if (GetBaseCharacterMovementComponent()->IsSprinting()) {
			StopSprint();
		}
		if (GetBaseCharacterMovementComponent()->IsProning()) {
			if (CanCrouch()) {
				float Height = GetDefaultCapsuleHeight();
				float Radius = GetDefaultCapsuleRadius();
				ChangeCapsuleParamOutProneState(Radius, Height);
				ChangeMaxSpeedOfPlayer(600.0f);
				GetBaseCharacterMovementComponent()->ChangeProneState();
			}
		}
		else {
			float Height = GetProneCapsuleHeight();
			float Radius = GetProneCapsuleRadius();
			if (GetBaseCharacterMovementComponent()->IsCrouched()) {
				GetBaseCharacterMovementComponent()->ChangeCrouchState();
				ChangeCapsuleParamOnProneStateFromCrouch(Radius, Height);
			}
			else
			{
				ChangeCapsuleParamOnProneState(Radius, Height);
			}
			ChangeMaxSpeedOfPlayer(100.0f);
			GetBaseCharacterMovementComponent()->ChangeProneState();
		}
	}
}
void AGCBaseCharacter::ChangeCapsuleParamOnProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (SpringArmComponent->GetRelativeLocation().Z + ProneCapsuleHalfHeight + 8)), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}
void AGCBaseCharacter::ChangeCapsuleParamOutProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.0f, 0.0f, (GetDefaultCapsuleHeight() - GetProneCapsuleHeight())), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	SpringArmComponent->SetRelativeLocation(SpringArmComponent->GetRelativeLocation() - FVector(0, 0, (GetProneCapsuleHeight() + 8)));
}
void AGCBaseCharacter::ChangeCapsuleParamOnProneStateFromCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCrouchCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, SpringArmComponent->GetRelativeLocation().Z - ProneCapsuleHalfHeight + 12), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}
void AGCBaseCharacter::ChangeCapsuleParamFromProneStateToCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCapsuleComponent()->GetRelativeLocation().Z - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, GetBaseCharacterMovementComponent()->CrouchedHalfHeight - ProneCapsuleHalfHeight), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineForIkFoot.TickTimeline(DeltaTime);
	TimelineForSkeletonPosition.TickTimeline(DeltaTime);
	TryChangeSprintState();
	//CalculateIkFootPosition();
#if ENABLE_DRAW_DEBUG
	InitIkDebugDraw();
#else
	bool bIsDebugEnable = false;
#endif
}

void AGCBaseCharacter::InitIkDebugDraw() {
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bIsDebugLkCalculationEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryIkCalculation);
}
void AGCBaseCharacter::Mantle(bool bForce)
{
	if (!CanMantle() && !bForce) {
		return;
	}
	FLedgeDescription LedgeDescription;
	if (LegDetectorComponent->DetectLedge(LedgeDescription) && !GetBaseCharacterMovementComponent()->IsMantling()){
		FMantlingMovementParameters MantlingParameters;
		if (LedgeDescription.HitObject != NULL && LedgeDescription.HitObject) {
			MantlingParameters.HitObject = LedgeDescription.HitObject;
		//	AttachToComponent(LedgeDescription.HitObject->GetPlatformMesh(),FAttachmentTransformRules::KeepWorldTransform);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Some debug message!"));
			
		}
		else
		{
			MantlingParameters.HitObject = NULL;
		}
		//MantlingParameters.InitialLocation = LedgeDescription.HitObject->GetActorTransform().TransformPosition(GetActorLocation());
		//else
			MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		//if (LedgeDescription.HitObject != NULL)
		//MantlingParameters.TargetLocation = LedgeDescription.HitObject->GetActorTransform().TransformPosition(LedgeDescription.Location);
		//else
			MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;

	
	//	float StartTime = HightMantleSettings.MaxHeightStartTime + (MantlingHeight = HightMantleSettings.MinHeight) / (HightMantleSettings.MaxHeight - HightMantleSettings.MinHeight) * (HightMantleSettings.MaxHeightStartTime - HightMantleSettings.MinHeightStartTime);
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;
		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.Duration -= MantlingParameters.StartTime;
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector+ MantlingSettings.AnimationCorrectionXY*LedgeDescription.LedgeNormal;
		GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f,EMontagePlayReturnType::Duration,MantlingParameters.StartTime);
    }
}
void AGCBaseCharacter::TryToRunWall()
{
	GetBaseCharacterMovementComponent()->TryToRunWall();
}
void AGCBaseCharacter::CalculateIkFootPosition()
{
	UGCBaseCharacterAnimInstance* GCBaseCharacterAnimInstance = Cast<UGCBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (!GCBaseCharacterAnimInstance)
		return;
		if (GetVelocity().Size() == 0 && !GetBaseCharacterMovementComponent()->IsMantling()) {
			TArray<AActor*>ActorsToIgnore;
			FHitResult LeftTraceHit;
			const FVector LeftFootLocation = GetTransform().TransformPosition(LeftFootBoneRelativeLocation);
			EDrawDebugTrace::Type DebugTrace;
			if (bIsDebugLkCalculationEnable)
				DebugTrace =EDrawDebugTrace::ForOneFrame;
			else
				DebugTrace = EDrawDebugTrace::None;
			bool bLeftFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(this, LeftFootLocation + FVector(0.f, 0.0f, 50.f),
				LeftFootLocation + FVector(0.f, 0.f, -100.f), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, DebugTrace, LeftTraceHit, true);

			FHitResult RightTraceHit;
			const FVector RightFootLocation = GetTransform().TransformPosition(RightFootBoneRelativeLocation);
			bool bRightFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(this, RightFootLocation + FVector(0.f, 0.0f, 50.f),
				RightFootLocation + FVector(0.f, 0.f, -100.f), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, DebugTrace, RightTraceHit, true);

			float ZOffset = 0;
			if (RightTraceHit.ImpactPoint.Z>0 && LeftTraceHit.ImpactPoint.Z >0 && fabs(RightTraceHit.ImpactPoint.Z - LeftTraceHit.ImpactPoint.Z) < 50.0f) {
				if (LeftTraceHit.ImpactPoint.Z < RightTraceHit.ImpactPoint.Z) {
					ZOffset = LeftFootLocation.Z - LeftTraceHit.ImpactPoint.Z;
					StartEffectorPosition = RightFootBoneRelativeLocation;
					FinalEffectorPosition = RightTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f);
					bChangeRightEffector = true;
					bChangeLeftEffector = false;
					//GCBaseCharacterAnimInstance->setRightEffectorLocation(RightTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f));
					GCBaseCharacterAnimInstance->SetLeftFootAlpha(0.f);
					GCBaseCharacterAnimInstance->SetRightFootAlpha(1.f);

					TimelineForIkFoot.Play();
				}
				else {
					ZOffset = RightFootLocation.Z - RightTraceHit.ImpactPoint.Z;
					//GCBaseCharacterAnimInstance->setLeftEffectorLocation(LeftTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f));
					bChangeRightEffector = false;
					bChangeLeftEffector = true;
					StartEffectorPosition = LeftFootBoneRelativeLocation;
					FinalEffectorPosition = LeftTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f);
					GCBaseCharacterAnimInstance->SetRightFootAlpha(0.f);
					GCBaseCharacterAnimInstance->SetLeftFootAlpha(1.f);
					if (RightTraceHit.ImpactPoint.Z == LeftTraceHit.ImpactPoint.Z)
						GCBaseCharacterAnimInstance->setLeftEffectorLocation(FinalEffectorPosition);
					else
						TimelineForIkFoot.Play();
				}
				StartSkeletonPosition = GetMesh()->GetRelativeLocation();
				if (GetBaseCharacterMovementComponent()->IsCrouched()) {
					ZOffset = -18.0f;
				}
				EndSkeletonPosition = InitialMeshRalativeLocation + FVector(0.f, 0.f, -ZOffset + 14.0f);
				if (GetBaseCharacterMovementComponent()->IsCrouched()) {
					ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, -ZOffset + 14.0f));
				}
				else if (fabs(LeftTraceHit.ImpactPoint.Z-RightTraceHit.ImpactPoint.Z)>1.0f && GetBaseCharacterMovementComponent()->IsProning()) {
					ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, -ZOffset + 60.0f));
				}
				else
					TimelineForSkeletonPosition.PlayFromStart();
			}
		}
		else if(!GetBaseCharacterMovementComponent()->IsProning() && !GetBaseCharacterMovementComponent()->IsCrouched()) {
			StartSkeletonPosition = GetMesh()->GetRelativeLocation();
			EndSkeletonPosition = InitialMeshRalativeLocation;
			if (GetBaseCharacterMovementComponent()->IsProning()) {
				EndSkeletonPosition += FVector(0.f, 0.f,1000.0f);
			}
			if (TimelineForSkeletonPosition.IsPlaying())
				TimelineForSkeletonPosition.Stop();
			if (TimelineForIkFoot.IsPlaying())
				TimelineForIkFoot.Stop();
			TimelineForSkeletonPosition.Stop();
			Cast<UGCBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetLeftFootAlpha(0.f);
			Cast<UGCBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetRightFootAlpha(0.f);
			ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
		}
}

void AGCBaseCharacter::OnDeath()
{
	GetBaseCharacterMovementComponent()->DisableMovement();
	float Duration=PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f) {
		EnableRagdoll();
	}
}
void AGCBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
}

void AGCBaseCharacter::ChangeCapsuleParamFromIdleWalkStateToCrouch()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetCrouchCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation  + FVector(0.f, 0.f, (GetCrouchCapsuleHeight() / 2)+7));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f,((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2)+19), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::ChangeCapsuleParamFromCrouchedToIdleWalk()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetDefaultCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, (GetDefaultCapsuleHeight())/2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (-((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight())/2)-19)), GetCapsuleComponent()->GetComponentQuat()
		, false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::InitTimelineToIKFoot()
{
	FOnTimelineFloatStatic IKFootTimeLineUpdate;
	IKFootTimeLineUpdate.BindUObject(this, &AGCBaseCharacter::IKFootPositionUpdate);
	TimelineForIkFoot.AddInterpFloat(TimelineCurveForIKFoot ,IKFootTimeLineUpdate);
	TimelineForIkFoot.SetTimelineLength(0.05f);
	TimelineForIkFoot.SetLooping(false);

	FOnTimelineFloatStatic IKSkeletonTimeLineUpdate;
	IKSkeletonTimeLineUpdate.BindUObject(this, &AGCBaseCharacter::IKSkeletonPositionUpdate);
	TimelineForSkeletonPosition.AddInterpFloat(TimelineCurveForIKFoot, IKSkeletonTimeLineUpdate);
	TimelineForSkeletonPosition.SetTimelineLength(0.05f);
	TimelineForSkeletonPosition.SetLooping(false);


}

void AGCBaseCharacter::IKFootPositionUpdate(float Alpha)
{
	FVector CurrentEffectorPosition = FMath::Lerp(StartEffectorPosition, FinalEffectorPosition, Alpha);
	if (bChangeRightEffector) {
		Cast<UGCBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance())->setRightEffectorLocation(CurrentEffectorPosition);
	}
	else if (bChangeLeftEffector) {
		Cast<UGCBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance())->setLeftEffectorLocation(CurrentEffectorPosition);
	}
	if (CurrentEffectorPosition == FinalEffectorPosition) {
		TimelineForIkFoot.Stop();
	}
}

void AGCBaseCharacter::IKSkeletonPositionUpdate(float Alpha)
{
	FVector CurrentEffectorPosition = FMath::Lerp(StartSkeletonPosition, EndSkeletonPosition, Alpha);
	if (CurrentEffectorPosition == EndSkeletonPosition)
		TimelineForSkeletonPosition.Stop();
	ChangeSkeletalMeshPosition(CurrentEffectorPosition);
}


void AGCBaseCharacter::ChangeSkeletalMeshPosition(FVector Position)
{
	GetMesh()->SetRelativeLocation(Position);
}

void AGCBaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void AGCBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex-GetActorLocation()).Z/100;
	if (IsValid(FallDamageCurve)) {
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GCBaseCharacterMovementComponent==nullptr) {
		GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());
	}
	if (GetMesh()->SkeletalMesh) {
		const FVector LeftFootBoneWorldLocation = GetMesh()->GetBoneLocation(LeftFootBoneName);
		LeftFootBoneRelativeLocation = GetActorTransform().InverseTransformPosition(LeftFootBoneWorldLocation);
		const FVector RightFootBoneWorldLocation = GetMesh()->GetBoneLocation(RightFootBoneName);
		RightFootBoneRelativeLocation = GetActorTransform().InverseTransformPosition(RightFootBoneWorldLocation);
		InitialMeshRalativeLocation = GetMesh()->GetRelativeTransform().GetLocation();
	}
	InitTimelineToIKFoot();
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this,&AGCBaseCharacter::OnDeath);
}
UGCBaseCharacterMovementComponent* AGCBaseCharacter::GetBaseCharacterMovementComponent() const
{
		return GCBaseCharacterMovementComponent;
}
AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());
	LegDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
	GetBaseCharacterMovementComponent()->bOrientRotationToMovement = true;
	CharacterAttributesComponent= CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));
}

void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AGCBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value)) {
		FVector LadderUpVector=GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector,Value);
	}
}

void AGCBaseCharacter::InteractionWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder()) {
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else {
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder)) {
			if (AvailableLadder->GetIsOnTop()) {
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}
void AGCBaseCharacter::InteractionWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline()) {
		GetBaseCharacterMovementComponent()->DetachFromZipline(EDetachFromLadderMethod::JumpOff);
	}
	else {
		AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline)) {
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

const ALadder* AGCBaseCharacter::GetAvailableLadder()
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors) {
		if (InteractiveActor->IsA<ALadder>()) {
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}
AZipline* AGCBaseCharacter::GetAvailableZipline()
{
	 AZipline* Result = nullptr;
	for (AInteractiveActor* InteractiveActor : AvailableInteractiveActors) {
		if (InteractiveActor->IsA<AZipline>()) {
			Result = StaticCast< AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}
bool AGCBaseCharacter::CanSprint()
{
	if (bIsCrouched || !bCanStartSrpint || GCBaseCharacterMovementComponent->IsProning())
		return false;
	return true;
}

void AGCBaseCharacter::OnSprintStart_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AGCBaseCharacter::OnSprintStart_Implementation"));
}

void AGCBaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AGCBaseCharacter::OnSprintEnd_Implementation"));
}

bool AGCBaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder() && !GetBaseCharacterMovementComponent()->IsOnZipline()  && !GetBaseCharacterMovementComponent()->IsSlide();
}

void AGCBaseCharacter::TryChangeSprintState()
{
	if (GetBaseCharacterMovementComponent() != nullptr) {
		if (bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting() && CanSprint()) {
			GetBaseCharacterMovementComponent()->StartSprint();
			OnSprintStart();
		}
		if (!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting()) {
			GetBaseCharacterMovementComponent()->StopSprint();
			OnSprintEnd();
		}
	}
}

const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() || GetBaseCharacterMovementComponent()->IsFlying()) {
		return LedgeHeight > LowMantleMaxHeight ? SwimmingMantleSettings : LowMantleSettings;
	}
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

bool AGCBaseCharacter::CanCrouch()
{
	TArray<AActor*>ActorsToIgnore;
	FHitResult TraceHit;
	const FVector RightFootLocation = GetTransform().TransformPosition(RightFootBoneRelativeLocation);
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(this, GetCapsuleComponent()->GetRelativeLocation(),
		GetCapsuleComponent()->GetRelativeLocation()+FVector(0.0f,0.0f,GetDefaultCapsuleHeight()+45), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, TraceHit, true);
	return !bIsHit && !GetBaseCharacterMovementComponent()->IsSlide() && !GetBaseCharacterMovementComponent()->IsSprinting();
}

void AGCBaseCharacter::ChangeCapsuleParamFromProneToCrouched()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetCrouchCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetProneCapsuleHeight() - GetCrouchCapsuleHeight())), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, (GetCrouchCapsuleHeight() / 2) + 7));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, ((GetProneCapsuleHeight() - GetCrouchCapsuleHeight()))), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const
{
	return(!GetBaseCharacterMovementComponent()->IsMantling() && !GetBaseCharacterMovementComponent()->IsSwimming() && !GetBaseCharacterMovementComponent()->IsOnLadder() && !GetBaseCharacterMovementComponent()->IsCrouched() && !GetBaseCharacterMovementComponent()->IsOnZipline());
}

