// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectorComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include <MyProject/GameCodeTypes.h>
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include <MyProject/GCGameInstance.h>
#include <MyProject/Utils/GCTraceUtils.h>
#include <MyProject/Subsystems/DebugSubsystem.h>
#include <MyProject/Characters/PlayerCharacter.h>



// Called when the game starts
void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT(" ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
	
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnable = false;
#endif
	float DrawTime = 2.0f;
	float BotoomZOffset = 2.0f;
	//FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight()- BotoomZOffset) * FVector::UpVector;
	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	FVector CharacterBottom;//= CachedCharacterOwner->GetActorLocation() - (CachedCharacterOwner->GetDefaultHalfHeight()- BotoomZOffset) * FVector::UpVector;
	if (CachedCharacterOwner->GetCharacterMovement()->IsSwimming()) {
		CharacterBottom= CachedCharacterOwner->GetActorLocation() - (CachedCharacterOwner->GetDefaultHalfHeight() - BotoomZOffset) * FVector::UpVector;
	}
	else {
		CharacterBottom= CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() - BotoomZOffset) * FVector::UpVector;
	}
	float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5f;
	//1. Forward check
	
	FVector ForwardStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;
	FHitResult ForwardCheckHitResult;
	if (!GCTraceUtils::SweepCapsuleSingleByChanel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, FQuat::Identity, ECC_Climbing,ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight,
		QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnable, DrawTime, FColor::Black, FColor::Red)
		) {
		return false;
	}

	//2.Downwarrd check
	FHitResult DownwardCheckHitResult;
	float DownwardSphereCheckRadius= CapsuleComponent->GetScaledCapsuleRadius();
	
	float DownwardCheckDepthOffset = 10.0f;
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardSphereCheckRadius;
	FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

	if (!GCTraceUtils::SweepSphereSingleByChanel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, FQuat::Identity, ECC_Climbing, DownwardSphereCheckRadius, QueryParams,FCollisionResponseParams::DefaultResponseParam, bIsDebugEnable,DrawTime)) {
		return false;
	}
	//3.Overlap check
	float OverlapCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	//float OverlapCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();//CachedCharacterOwner->GetDefaultHalfHeight();
	float OverlapCapsuleHalfHeight;//= CachedCharacterOwner->GetDefaultHalfHeight();//CachedCharacterOwner->GetDefaultHalfHeight();
	if (CachedCharacterOwner->GetCharacterMovement()->IsSwimming()) {
		OverlapCapsuleHalfHeight = CachedCharacterOwner->GetDefaultHalfHeight();
	}
	else {
		OverlapCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	}
	float OverlapCapsuleFloorOffset = 2.0f;
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight+OverlapCapsuleFloorOffset) * FVector::UpVector;
	if (GCTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius,OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnable, DrawTime)) {
	  return false;
	}
	if (CachedCharacterOwner->GetActorLocation().X - OverlapLocation.X == 0 && CachedCharacterOwner->GetActorLocation().Y - OverlapLocation.Y == 0) {
		return false;
	}
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal*FVector(-1.0f,-1.0f,0)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;
	//if (DownwardCheckHitResult.Actor->IsA<ABasePlatform>())
		LedgeDescription.HitObject = StaticCast<ABasePlatform*>(DownwardCheckHitResult.Actor.Get());
	//else
		LedgeDescription.HitObject = NULL;
	return true;
}
