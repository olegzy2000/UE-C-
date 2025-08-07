// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponFireMode :uint8 
{
	Single,
	FullAuto
};
UENUM(BlueprintType)
enum class EReloadType :uint8 {
	FullClip,
	ByBullet
};
DECLARE_MULTICAST_DELEGATE(FOnReloadComplete)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged,int32)
class UAnimMontage;
UCLASS(Blueprintable)
class MYPROJECT_API ARangeWeaponItem : public AEquipableItem ,public ISaveSubsystemInterface
{
	GENERATED_BODY()
public:
	ARangeWeaponItem();
	bool IsFiring() const;
	bool IsReloading() const;
	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	void SetAmmo(int32 NewAmmo);
	bool CanShoot() const;
	float GetAimFOV() const;
	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;
	float GetAimMovementMaxSpeed() const;
	FTransform GetForGribTransform() const;
	FOnAmmoChanged OnAmmoChanged;
	void StartReload();
	void OnShotTimerElapsed();
	void EndReload(bool bIsSuccess);
	FOnReloadComplete OnReloadComplete;
	virtual EReticleType GetReticleType() const;
	void ChangeFireMode();
	virtual void OnLevelDeserialized_Implementation() override;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
		EReticleType AimReticleType = EReticleType::Default;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWeaponBarellComponent* WeaponBarell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
		UAnimMontage* WeaponFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
		UAnimMontage* WeaponReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
		UAnimMontage* CharacterReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
		UAnimMontage* CharacterFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
		EWeaponFireMode FireMode=EWeaponFireMode::Single;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
		float RateFire = 0.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
		float SpreadAngle = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
		float AimSpreadAngle = 0.25f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming")
		float AimMovementMaxSpeed = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming")
		float AimFOV = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
		float AimTurnModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
		float AimLookUpModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
		bool bAutoReload = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
		int32 MaxAmmoToAlternativeShoting = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
		int32 MaxAmmoToDefaultShoting = 30;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
		EReloadType ReloadType = EReloadType::FullClip;


private:
	int32 MaxAmmo;
	UPROPERTY(SaveGame)
	int32 CurrentDefaultAmmo;
	UPROPERTY(SaveGame)
	int32 CurrentAlternativeAmmo;
	bool bIsFiring = false;
	bool bIsReloading = false;
	EWeaponFireMode DefaultFireMode;
	float GetCurrentBulletSpreadAngle() const;
	void MakeShot();
	float PlayAnimMontage(UAnimMontage* AnimMontage);
	void StopAnimMontage(UAnimMontage* AnimMontage,float BlendOutTime=0.0);
	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;
	bool bIsAiming;
};
