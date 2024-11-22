// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectile/GCProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
class UExplosionComponent;
UCLASS()
class MYPROJECT_API AExplosiveProjectile : public AGCProjectile
{
	GENERATED_BODY()
public:
	AExplosiveProjectile();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UExplosionComponent* ExplosionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
		float DetonationTime=2.0f;
	virtual void OnProjectileLaunch() override;
private:
	void OnDetonationTimerElapsed();
	FTimerHandle DetonationTimer;
	AController* GetController();
	
};
