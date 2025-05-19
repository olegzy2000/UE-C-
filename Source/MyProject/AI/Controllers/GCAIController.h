// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GCAIController.generated.h"

/**
 * 
 */
class UAISense;
UCLASS()
class MYPROJECT_API AGCAIController : public AAIController
{
	GENERATED_BODY()
public:
	AGCAIController();
protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense>SensedClass) const;
private:

};
