// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCodeTypes.h"
//#include "Characters/GCBaseCharacter.h"
ARangeWeaponItem::ARangeWeaponItem() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzleSocket);
	ReticleType = EReticleType::Default;
	EquppedSocketName = SocketCharacterWeapon;

}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegress = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegress);
}

void ARangeWeaponItem::MakeShot()
{
	AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
	if (!IsValid(CurrentCharacterOwner))
		return;
	if (!CanShoot()) {
		StopFire();
		if (GetCurrentAmmo() == 0 && bAutoReload) {
			CurrentCharacterOwner->Reload();
		}
		return;
	}
	EndReload(false);
	CurrentCharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);
	FVector ShotLocation;
	FRotator ShotRotation;
	if (CurrentCharacterOwner->IsPlayerControlled()) {
		APlayerController* Controller = CurrentCharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(ShotLocation, ShotRotation);
	}
	else {
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CurrentCharacterOwner->GetBaseAimRotation();
	}
	
	FVector ShotDirection= ShotRotation.RotateVector(FVector::ForwardVector);
	
	WeaponBarell->Shot(ShotLocation, ShotDirection, GetCurrentBulletSpreadAngle());
	SetAmmo(GetCurrentAmmo() - 1);
	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, RateFire, false);
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

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	Super::SetAmmo(NewAmmo);
	if (OnAmmoChanged.IsBound()) {
		OnAmmoChanged.Broadcast(GetCurrentAmmo());
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return GetCurrentAmmo()>0;
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
	FTransform GribSocketTransform;
	if (WeaponMesh && WeaponForGribSocket.IsValid()) { 
		const USkeletalMeshSocket* SkeletalMeshSocket = WeaponMesh->GetSocketByName(WeaponForGribSocket);
		if(SkeletalMeshSocket)
		GribSocketTransform = WeaponMesh->GetSocketTransform(WeaponForGribSocket);
	}
	return GribSocketTransform;
}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be owner of range weapon"));
	AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
	if (!IsValid(CurrentCharacterOwner))
		return;
	bIsReloading = true;
	if (IsValid(CharacterReloadMontage)) {
		float MontageDuration=CurrentCharacterOwner->PlayAnimMontage(CharacterReloadMontage);
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
		AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
		if (!IsValid(CurrentCharacterOwner))
			return;
		CurrentCharacterOwner->StopAnimMontage(CharacterReloadMontage);
		StopAnimMontage(WeaponReloadMontage);
	}
	if (ReloadType==EReloadType::ByBullet) {
		AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
		if (!IsValid(CurrentCharacterOwner))
			return;
		UAnimInstance* CharacterAnimInstance = CurrentCharacterOwner->GetMesh()->GetAnimInstance();
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

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming?AimReticleType:Super::GetReticleType();
}

void ARangeWeaponItem::ChangeFireMode()
{
	if (!WeaponBarell->CanUseRifleGrenate())
		return;
	if (WeaponBarell->UseRifleGrenate()) {
		FireMode = DefaultFireMode;
		AmmoType = DefaultAmmoType;
		CurrentAlternativeAmmo = GetCurrentAmmo();
		MaxAmmo = MaxAmmoToDefaultShoting;
		SetMaxAmmo(MaxAmmoToDefaultShoting);
		SetAmmo(CurrentDefaultAmmo);
	}
	else {
		DefaultFireMode = FireMode;
		FireMode = EWeaponFireMode::Single;
		AmmoType = EAmunitionType::RifleGrenete;
		CurrentDefaultAmmo = GetCurrentAmmo();
		SetMaxAmmo(MaxAmmoToAlternativeShoting);
		MaxAmmo = MaxAmmoToAlternativeShoting;
		SetAmmo(CurrentAlternativeAmmo);
	}
	WeaponBarell->ChangeUseRifleGrenate();
	WeaponBarell->ChangeCurrentProjectileClass();
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	//if()
	DefaultAmmoType = AmmoType;
	SetMaxAmmo(MaxAmmoToDefaultShoting);
	MaxAmmo = MaxAmmoToDefaultShoting;
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

bool ARangeWeaponItem::IsFiring() const {
	return bIsFiring;
}
bool ARangeWeaponItem::IsReloading() const {
	return bIsReloading;
}