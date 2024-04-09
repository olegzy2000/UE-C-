// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/Actor.h"
#include "SwimmingPool.generated.h"

UCLASS()
class MYPROJECT_API ASwimmingPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwimmingPool();
	UPROPERTY(EditAnywhere)
		APhysicsVolume* physicsVolumel;

};
