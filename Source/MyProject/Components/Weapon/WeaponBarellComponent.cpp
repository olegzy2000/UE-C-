// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/WeaponBarellComponent.h"
#include "GameCodeTypes.h"
#include "Subsystems/DebugSubsystem.h"
#include "../../Actors/Projectile/GCProjectile.h"
#include "../../Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle,bool IsAming)
{
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	for (int i = 0; i < BulletsPerShot; i++) {
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnable = false;
#endif
		switch (HitRegistration)
		{
		case EHitRegistrationType::HitScan: {
			
			bool bHasHit=HitScan(ShotStart, ShotEnd, ShotDirection);
			if (bIsDebugEnable && bHasHit) {
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Blue, false, 1.0f);
				DrawDebugLine(GetWorld(), ShotStart, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
			}
			break;
		}
		case EHitRegistrationType::Projectile: {
			FHitResult ShotResult;
			bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
			if (bHasHit) {
				ShotEnd = ShotResult.ImpactPoint;
			}
			FVector CurrentStartLocation = MuzzleLocation;
			if (CachedRangeWeaponItem->GetReticleType() == EReticleType::SniperRifle && IsAming)
				CurrentStartLocation = ShotStart;
			FVector ShotDirectionProjectile = ShotEnd - CurrentStartLocation;

			
			if (bIsDebugEnable && bHasHit) {
				DrawDebugLine(GetWorld(), CurrentStartLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
			}
			LaunchProjectile(CurrentStartLocation, ShotDirectionProjectile);
			break;
		}
		default:
			break;
		}
	}
}
void UWeaponBarellComponent::ChangeCurrentProjectileClass()
{
	if (bUseRifleGrenete)
		CurrentProjectileClass = RifleGreneteClass;
	else
		CurrentProjectileClass = ProjectileClass;
}
bool UWeaponBarellComponent::CanUseRifleGrenate()
{
	return bCanUseRifleGrenate;
}
bool UWeaponBarellComponent::UseRifleGrenate()
{
	return bUseRifleGrenete;
}
void UWeaponBarellComponent::ChangeUseRifleGrenate() {
	bUseRifleGrenete = !bUseRifleGrenete;
	if (bUseRifleGrenete) {
		OldHitRegistration = HitRegistration;
		HitRegistration = EHitRegistrationType::Projectile;
	}
	else{
		HitRegistration = OldHitRegistration;
	}

}
TSubclassOf<AGCProjectile> UWeaponBarellComponent::GetCurrentProjectileClass() const
{
	return CurrentProjectileClass;
}
TSubclassOf<AGCProjectile> UWeaponBarellComponent::GetRifleGreneteClass() const
{
	return RifleGreneteClass;
}
TSubclassOf<AGCProjectile> UWeaponBarellComponent::GetProjectileClass() const
{
	return ProjectileClass;
}
bool UWeaponBarellComponent::HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector ShotDirection) {
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit) {
		ShotEnd = ShotResult.ImpactPoint;
		ProcessHit(ShotResult,ShotDirection);
	}
	UNiagaraComponent* TraceFxComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, GetComponentLocation(), GetComponentRotation());
	TraceFxComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
	return bHasHit;
}
void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, FVector LaunchDirection)
{
	AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(CurrentProjectileClass, LaunchStart, LaunchDirection.ToOrientationRotator());
	if (IsValid(Projectile)) {
		Projectile->SetOwner(GetOwningPawn());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}
void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction) {
	if (bUseRifleGrenete) {
		HitByRifleGrenete();
	}
	else {
		HitByDefaultBullet(HitResult, Direction);
	}
}
void UWeaponBarellComponent::HitByRifleGrenete() {
	
}
void UWeaponBarellComponent::HitByDefaultBullet(const FHitResult& HitResult, const FVector& Direction) {
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor) && FalloffDiagram!=nullptr && IsValid(FalloffDiagram)) {
		float DamageCoef = FalloffDiagram->GetFloatValue(HitResult.Distance);//Distance
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage(DamageAmount * DamageCoef, DamageEvent, GetController(), GetOwner());
	}
	if (IsValid(DefaultDecalInfo.DecalMaterial)) {
		UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
		if (IsValid(DecalComponent)) {
			DecalComponent->SetFadeScreenSize(0.001f);
			DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime, true);
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("UWeaponBarellComponent::Shot Decal material not found"));
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
    ChangeCurrentProjectileClass();
	if(GetOwner()->IsA<ARangeWeaponItem>())
	CachedRangeWeaponItem = Cast<ARangeWeaponItem>(GetOwner());
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

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner)) {
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner=GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

EHitRegistrationType UWeaponBarellComponent::GetHitRegistration() {
	return HitRegistration;
}