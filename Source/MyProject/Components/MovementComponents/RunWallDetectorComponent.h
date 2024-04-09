// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Components/ActorComponent.h"
#include "../../Subsystems/DebugSubsystem.h"
#include "RunWallDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FRunWallDescription
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		FVector Location;
	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		FVector EndLocation;

	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		FVector Velocity;

	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		FRotator Rotation;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		FHitResult OutHit;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run wall description")
		bool bIsLeft;

		bool CanRun = false;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API URunWallDetectorComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RunWallDetectorComponent : Distance")
		float Distance = 75.0f;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
	//UDebugSubsystem DebugSystem;
public:
	FRunWallDescription DetectionWall();

		
};
