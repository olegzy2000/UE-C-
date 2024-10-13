// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCodeTypes.h"
#include "Characters/GCBaseCharacter.h"
ARangeWeaponItem::ARangeWeaponItem() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzleSocket);
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegress = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegress);
}

void ARangeWeaponItem::MakeShot()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::Fire() only character can be owner of range weapon"));
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller)) {
		return;
	}
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint,PlayerViewRotation);
	FVector ViewDirection= PlayerViewRotation.RotateVector(FVector::ForwardVector);
	ViewDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f,GetCurrentBulletSpreadAngle()),PlayerViewRotation);
	WeaponBarell->Shot(PlayerViewPoint, ViewDirection, Controller);
}

void ARangeWeaponItem::StartFire()
{
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
	MakeShot();
	if(FireMode == EWeaponFireMode::FullAuto)
	GetWorld()->GetTimerManager().SetTimer(ShotTimer,this, &ARangeWeaponItem::MakeShot, TimeBeetwenFire,true);
}

void ARangeWeaponItem::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

FTransform ARangeWeaponItem::GetForGribTransform() const
{
	return WeaponMesh->GetSocketTransform(WeaponForGribSocket);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f;
	if (IsValid(WeaponAnimInstance)) {
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ= FMath::Sin(RotationAngle);
	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY)*SpreadSize;
	return Result;
}
