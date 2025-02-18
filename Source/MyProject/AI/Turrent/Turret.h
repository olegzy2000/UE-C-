// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Components/AlAttributeComponent.h"
#include <GameCodeTypes.h>
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8 {
	Searching,
	Firing,
	Dead
};
class UWeaponBarellComponent;
UCLASS()
class MYPROJECT_API ATurret : public APawn
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* TurretBaseComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* TurretBarellComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWeaponBarellComponent* WeaponBarell;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UAlAttributeComponent* AlAttributeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BaseSearchingRotationRate = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BarellPitchRotationRate = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxBarellPitchAngle = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinBarellPitchAngle = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BaseFiringInterpSpeed = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters | fire", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float RateOfFire = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters | fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BulletSpreadAngle = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters | fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float FireDelayTime = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters | Team")
		ETeams Team=ETeams::Enemy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turrent parameters | Explosion | VFX")
		UParticleSystem* ExplosionVFX;

public:
	virtual void PossessedBy(AController* NewController) override;
	// Sets default values for this pawn's properties
	ATurret();
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetCurrentTarget(AActor* NewTarget);
	AActor* GetCurrentTarget();
	virtual FVector GetPawnViewLocation()const override;
	virtual FRotator GetViewRotation() const override;
private:
	void MakeShot();
	UFUNCTION()
	void OnTakeAnyDamageCustom(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	void SearchingMovement(float DeltaTime);
	void SetCurrentTurretState(ETurretState NewState);
	void FiringMovement(float DeltaTime);
	ETurretState CurrentTurretState = ETurretState::Searching;
	AActor* CurrentTarget = nullptr;
	FTimerHandle ShotTimer;
	float GetFireInterval()const;
};
