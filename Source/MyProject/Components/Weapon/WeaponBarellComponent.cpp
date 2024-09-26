// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/WeaponBarellComponent.h"
#include "GameCodeTypes.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UWeaponBarellComponent::Shot(FVector ShotStart,FVector ShotDirection,AController* Controller)
{
	FVector MuzzleLocation = GetComponentLocation();
	FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
	FHitResult ShotResult;
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryRangeWeapon);
#else
	bool bIsDebugEnable = false;
#endif

	if (GetWorld()->LineTraceSingleByChannel(ShotResult, MuzzleLocation,ShotEnd, ECC_Bullet)) {
		ShotEnd = ShotResult.ImpactPoint;
		AActor* HitActor = ShotResult.GetActor();
		if (IsValid(HitActor)) {
			HitActor->TakeDamage(DamageAmount, FDamageEvent{}, Controller, GetOwner());
		}
		if (bIsDebugEnable) {
			DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Blue,false,1.0f);
		}
	}
	if (bIsDebugEnable) {
		DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
}
