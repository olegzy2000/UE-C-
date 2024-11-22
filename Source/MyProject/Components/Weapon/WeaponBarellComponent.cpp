// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/WeaponBarellComponent.h"
#include "GameCodeTypes.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UWeaponBarellComponent::Shot(FVector ShotStart,FVector ShotDirection,AController* Controller,float SpreadAngle)
{
	for (int i = 0; i < BulletsPerShot; i++) {
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector MuzzleLocation = GetComponentLocation();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
		FHitResult ShotResult;
#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnable = false;
#endif

		if (GetWorld()->LineTraceSingleByChannel(ShotResult, MuzzleLocation, ShotEnd, ECC_Bullet)) {
			ShotEnd = ShotResult.ImpactPoint;
			AActor* HitActor = ShotResult.GetActor();
			if (IsValid(HitActor)) {
				float Distance = FVector::Dist(MuzzleLocation, ShotEnd);
				float DamageCoef = FalloffDiagram->GetFloatValue(Distance);
				FPointDamageEvent DamageEvent;
				DamageEvent.HitInfo = ShotResult;
				DamageEvent.ShotDirection = ShotDirection;
				DamageEvent.DamageTypeClass = DamageTypeClass;
				HitActor->TakeDamage(DamageAmount * DamageCoef, DamageEvent, Controller, GetOwner());
			}
			if (bIsDebugEnable) {
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Blue, false, 1.0f);
			}
		}
		if (IsValid(DefaultDecalInfo.DecalMaterial)) {
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, ShotResult.ImpactPoint, ShotResult.ImpactNormal.ToOrientationRotator());
			if (IsValid(DecalComponent)) {
				DecalComponent->SetFadeScreenSize(0.001f);
				DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime, true);
			}
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("UWeaponBarellComponent::Shot Decal material not found"));
		}
		UNiagaraComponent* TraceFxComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFxComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
		if (bIsDebugEnable) {
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}
void UWeaponBarellComponent::InitFalloffDiagram()
{
	FalloffDiagram = NewObject<UCurveFloat>();
	FKeyHandle KeyHandleForProgressBar = FalloffDiagram->FloatCurve.AddKey(FiringRange, 0.f);
	FalloffDiagram->FloatCurve.AddKey(0.f, 1.0f);
	FalloffDiagram->FloatCurve.SetKeyInterpMode(KeyHandleForProgressBar, ERichCurveInterpMode::RCIM_Linear, true);
}
void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();
	InitFalloffDiagram();
}
FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);
	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
	return Result;
}

