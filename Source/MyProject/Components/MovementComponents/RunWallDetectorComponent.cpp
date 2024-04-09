// Fill out your copyright notice in the Description page of Project Settings.


#include "RunWallDetectorComponent.h"
#include <MyProject/Utils/GCTraceUtils.h>
#include <MyProject/GameCodeTypes.h>
void URunWallDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT(" ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());

}

FRunWallDescription URunWallDetectorComponent::DetectionWall()
{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool isEnableDebugDraw = DebugSubSystem->IsCategoryEnable(DebugCategoryRunWallDetector);
		FRunWallDescription RunWallDescription;
		QueryParams.bTraceComplex = true;
		QueryParams.AddIgnoredActor(GetOwner());
		if (GCTraceUtils::LineTraceSingleByChannel(GetWorld(), HitResult, CachedCharacterOwner->GetActorLocation(), CachedCharacterOwner->GetActorLocation() + (CachedCharacterOwner->GetActorRightVector() * Distance), ECC_RunWall, QueryParams, isEnableDebugDraw, 10, FColor::Red)) {
			FVector DirectionVector = FVector::CrossProduct(FVector::UpVector,HitResult.Normal).GetSafeNormal();
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
			FVector DirectionVector = FVector::CrossProduct(HitResult.Normal,FVector::UpVector).GetSafeNormal();
			RunWallDescription.Rotation = DirectionVector.ToOrientationRotator();
			RunWallDescription.Location = (HitResult.ImpactPoint+ CachedCharacterOwner->GetActorRightVector() * (CachedCharacterOwner->GetSimpleCollisionRadius()));
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
