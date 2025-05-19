// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"
UENUM(BlueprintType)
enum class FPatrollingType : uint8 {
	Circle,
	PingPong
};
class APatrollingPath;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()
public :
	bool CanPatrol();
	FVector GetClosestWayPoint();
	FVector SelectNextWayPoint();

protected:	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
		APatrollingPath* PatrollingPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
		FPatrollingType PatrollingType= FPatrollingType::Circle;
private:
	int32 CurrentIndexWayPoint=-1;
	bool bReturnToStart = false;
};
