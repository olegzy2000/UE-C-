// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PlatformInvocator.h"
#include "Components/TimelineComponent.h"
#include <MyProject/Platformes/Component/PlatformInvocatorComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8 {
	OnDemand = 0,
	Loop
};
UCLASS()
class MYPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();
	FTimerHandle getTimeHandler() {
		return FuzeTimerHandle;
	}

	UStaticMeshComponent* GetPlatformMesh() {
		return PlatformMesh;
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlatformTimeluneUpdate(float Alpha);
	UFUNCTION(BlueprintCallable)
	void StartMovingPlatform();
	void StopMovingPlatform();
	FTimeline PlatformTimeline;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToStop=5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UPlatformInvocatorComponent* PlatormInvocator;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector GetDeltaMoving();
private:
	FTimerHandle FuzeTimerHandle;

	void InitTimeline();
	FVector DeltaMoving=FVector(0,0,0);
};
