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
	EquppedSocketName = SocketCharacterWeapon;
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
	if (!CanShoot()) {
		StopFire();
		if (GetAmmo() == 0 && bAutoReload) {
			CharacterOwner->Reload();
		}
		return;
	}
	EndReload(false);
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
	
	WeaponBarell->Shot(PlayerViewPoint, ViewDirection, Controller,GetCurrentBulletSpreadAngle());
	SetAmmo(GetAmmo() - 1);
	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, TimeBeetwenFire, false);
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer)) {
		return;
	}
	bIsFiring = true;
	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
	//GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

//int32 ARangeWeaponItem::GetMaxAmmo() const
//{
//	return MaxAmmo;
//}

//int32 ARangeWeaponItem::GetAmmo() const
//{
//	return Ammo;
//}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	Super::SetAmmo(NewAmmo);
	if (OnAmmoChanged.IsBound()) {
		OnAmmoChanged.Broadcast(GetAmmo());
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return GetAmmo()>0;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	return AimTurnModifier;
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

FTransform ARangeWeaponItem::GetForGribTransform() const
{
	return WeaponMesh->GetSocketTransform(WeaponForGribSocket);
}

//EAmunitionType ARangeWeaponItem::GetAmmoType() const
//{
//	return AmmoType;
//}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be owner of range weapon"));
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	bIsReloading = true;
	if (IsValid(CharacterReloadMontage)) {
		float MontageDuration=CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if (ReloadType == EReloadType::FullClip) {
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() {EndReload(true); }, MontageDuration, false);
		}
	}
	else {
		EndReload(true);
	}
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring) {
		return;
	}
	switch (FireMode) {
	  case EWeaponFireMode::Single: {
		  StopFire();
		  break;
	  }
	  case EWeaponFireMode::FullAuto: {
		 MakeShot();
	   }
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsReloading) {
		return;
	}
	if (!bIsSuccess) {
		checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be owner of range weapon"));
		AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
		CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		StopAnimMontage(WeaponReloadMontage);
	}
	if (ReloadType==EReloadType::ByBullet) {
		AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance)) {
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd,CharacterReloadMontage);
		}
		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance)) {
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	bIsReloading = false;
	if (bIsSuccess && OnReloadComplete.IsBound()) {
		OnReloadComplete.Broadcast();
	}
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
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

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f;
	if (IsValid(WeaponAnimInstance)) {
		WeaponAnimInstance->Montage_Stop(BlendOutTime,AnimMontage);
	}
}

