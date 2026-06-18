// Fill out your copyright notice in the Description page of Project Settings.


#include "RunWallDetectorComponent.h"
#include "MyProject.h"
#include "Engine/GameInstance.h"
#include <MyProject/Utils/GCTraceUtils.h>
#include <MyProject/GameCodeTypes.h>
void URunWallDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CachedCharacterOwner.IsValid())
	{
		UE_LOG(LogTraversal, Warning, TEXT("URunWallDetectorComponent::BeginPlay failed: owner is not ACharacter | Owner=%s"), *GetNameSafe(GetOwner()));
	}
}

FRunWallDescription URunWallDetectorComponent::DetectionWall()
{
	FRunWallDescription RunWallDescription;
	RunWallDescription.CanRun = false;

	if (!CachedCharacterOwner.IsValid() || !IsValid(GetWorld()))
	{
		return RunWallDescription;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UDebugSubsystem* DebugSubSystem = IsValid(GameInstance) ? GameInstance->GetSubsystem<UDebugSubsystem>() : nullptr;
	bool isEnableDebugDraw = IsValid(DebugSubSystem) && DebugSubSystem->IsCategoryEnable(DebugCategoryRunWallDetector);
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	if (GCTraceUtils::LineTraceSingleByChannel(GetWorld(), HitResult, CachedCharacterOwner->GetActorLocation(), CachedCharacterOwner->GetActorLocation() + (CachedCharacterOwner->GetActorRightVector() * Distance), ECC_RunWall, QueryParams, isEnableDebugDraw, 10, FColor::Red)) {
		FVector DirectionVector = FVector::CrossProduct(FVector::UpVector, HitResult.Normal).GetSafeNormal();
		RunWallDescription.Rotation = DirectionVector.ToOrientationRotator();
		RunWallDescription.Location = (HitResult.ImpactPoint + CachedCharacterOwner->GetActorRightVector() * (-CachedCharacterOwner->GetSimpleCollisionRadius()));
		FVector DeltaVelocity = CachedCharacterOwner.Get()->GetActorForwardVector() * 700;
		FVector NewVelocity = FVector(DeltaVelocity.X, DeltaVelocity.Y, -50);
		RunWallDescription.Velocity = NewVelocity;
		RunWallDescription.OutHit = HitResult;
		RunWallDescription.bIsLeft = false;
		CachedCharacterOwner.Get()->GetCharacterMovement();
		RunWallDescription.CanRun = true;
	}
	else if (GCTraceUtils::LineTraceSingleByChannel(GetWorld(), HitResult, CachedCharacterOwner->GetActorLocation(), CachedCharacterOwner->GetActorLocation() + (CachedCharacterOwner->GetActorRightVector() * (-Distance)), ECC_RunWall, QueryParams, isEnableDebugDraw, 10, FColor::Red)) {
		FVector DirectionVector = FVector::CrossProduct(HitResult.Normal, FVector::UpVector).GetSafeNormal();
		RunWallDescription.Rotation = DirectionVector.ToOrientationRotator();
		RunWallDescription.Location = (HitResult.ImpactPoint + CachedCharacterOwner->GetActorRightVector() * (CachedCharacterOwner->GetSimpleCollisionRadius()));
		RunWallDescription.OutHit = HitResult;
		RunWallDescription.bIsLeft = true;
		FVector DeltaVelocity = CachedCharacterOwner.Get()->GetActorForwardVector() * 700;
		FVector NewVelocity = FVector(DeltaVelocity.X, DeltaVelocity.Y, -50);
		RunWallDescription.Velocity = NewVelocity;
		CachedCharacterOwner.Get()->GetCharacterMovement();
		RunWallDescription.CanRun = true;
	}
	else
	{
		RunWallDescription.CanRun = false;
	}

	return RunWallDescription;
}
