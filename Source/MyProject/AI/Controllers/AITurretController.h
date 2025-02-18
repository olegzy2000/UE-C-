// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AITurretController.generated.h"

/**
 * 
 */
class ATurret;
UCLASS()
class MYPROJECT_API AAITurretController : public AAIController
{
	GENERATED_BODY()
public:
	AAITurretController();
	virtual void SetPawn(APawn* InPawn)override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
protected:
	virtual void BeginPlay() override;
private:
	TWeakObjectPtr<ATurret>CachedTurret;
};
