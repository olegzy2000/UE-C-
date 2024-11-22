// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	virtual void BeginPlay() override;
	void Shot(FVector ShotStart, FVector ShotDirection, AController* Controller,float SpreadAngle);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes",meta = (ClampMin=1,UIMin=1))
		int32 BulletsPerShot = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
		float FiringRange = 5000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
		TSubclassOf<class UDamageType> DamageTypeClass ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
		float DamageAmount = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
		class UNiagaraSystem* MuzzleFlashFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
		class UNiagaraSystem* TraceFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Decals")
		FDecalInfo DefaultDecalInfo;
private:
	void InitFalloffDiagram();
	UCurveFloat* FalloffDiagram;
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
};
