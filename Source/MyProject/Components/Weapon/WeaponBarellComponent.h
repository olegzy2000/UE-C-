// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"

UENUM(BlueprintType)
enum class EHitRegistrationType :uint8
{
	HitScan,
	Projectile
};

USTRUCT(BlueprintType)
struct FDecalInfo {
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
		UMaterialInterface* DecalMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
		FVector DecalSize=FVector(5.0f,5.0f,5.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
		float DecalLifeTime = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
		float DecalFadeOutTime = 5.0f;

};
class AGCProjectile;
class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	virtual void BeginPlay() override;
	void Shot(FVector ShotStart, FVector ShotDirection,float SpreadAngle, bool IsAming);
	void ChangeCurrentProjectileClass();
	bool CanUseRifleGrenate();
	bool UseRifleGrenate();
	void ChangeUseRifleGrenate();
	TSubclassOf<class AGCProjectile> GetRifleGreneteClass() const;
	TSubclassOf<class AGCProjectile> GetProjectileClass() const;
	TSubclassOf<class AGCProjectile> GetCurrentProjectileClass() const;
	EHitRegistrationType GetHitRegistration();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes",meta = (ClampMin=1,UIMin=1))
		int32 BulletsPerShot = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
		float FiringRange = 5000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
		TSubclassOf<class UDamageType> DamageTypeClass ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Hit registration")
		EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Hit registration" , meta=(EditCondition="HitRegistration==EHitRegistrationType::Projectile"))
		TSubclassOf<class AGCProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Alternative shoting")
		bool bCanUseRifleGrenate = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Alternative shoting", meta = (EditCondition = "bCanUseRifleGrenate"))
		TSubclassOf<class AGCProjectile> RifleGreneteClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
		float DamageAmount = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
		class UNiagaraSystem* MuzzleFlashFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
		class UNiagaraSystem* TraceFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Decals")
		FDecalInfo DefaultDecalInfo;
	UPROPERTY()
	UCurveFloat* FalloffDiagram;
private:
	TSubclassOf<class AGCProjectile> CurrentProjectileClass;
	void InitFalloffDiagram();
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
	APawn* GetOwningPawn() const;
	ARangeWeaponItem* CachedRangeWeaponItem;
	AController* GetController() const;
	bool HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector ShotDirection);
	void LaunchProjectile(const FVector& LaunchStart, FVector LaunchDirection);
	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
	void HitByRifleGrenete();
	void HitByDefaultBullet(const FHitResult& HitResult, const FVector& Direction);
	bool bUseRifleGrenete = false;
	EHitRegistrationType OldHitRegistration;
};
