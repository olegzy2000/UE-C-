// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/WeaponBarellComponent.h"
#include "GameCodeTypes.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UWeaponBarellComponent::Shot()
{
	FVector ShotStart = GetComponentLocation();
	FVector ShotDirection = GetComponentRotation().RotateVector(FVector::ForwardVector);
	FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
	FHitResult ShotResult;
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryRangeWeapon);
#else
	bool bIsDebugEnable = false;
#endif

	if (GetWorld()->LineTraceSingleByChannel(ShotResult,ShotStart,ShotEnd, ECC_Bullet)) {
		FVector HitLocation = ShotResult.ImpactPoint;
		if (bIsDebugEnable) {
			DrawDebugSphere(GetWorld(), HitLocation, 10.0f, 24, FColor::Blue,false,1.0f);
		}
	}
	if (bIsDebugEnable) {
		DrawDebugLine(GetWorld(), ShotStart, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
}
