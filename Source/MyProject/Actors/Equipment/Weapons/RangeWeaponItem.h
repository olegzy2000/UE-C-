// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/Weapon/WeaponBarellComponent.h"
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

class UAnimMontage;
UCLASS(Blueprintable)
class MYPROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	ARangeWeaponItem();
	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	float GetAimFOV() const;
	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;
	float GetAimMovementMaxSpeed() const;
	FTransform GetForGribTransform() const;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWeaponBarellComponent* WeaponBarell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
		UAnimMontage* WeaponFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
		UAnimMontage* CharacterFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
		EWeaponFireMode FireMode=EWeaponFireMode::Single;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.0f, UIMin=1.0f))
		float RateFire = 600.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float TimeBeetwenFire = 0.1f;
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
private:
	float GetCurrentBulletSpreadAngle() const;
	void MakeShot();
	float PlayAnimMontage(UAnimMontage* AnimMontage);
	FTimerHandle ShotTimer;
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
	bool bIsAiming;
};
