// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Actors/Interactive/InteractiveActor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "TrapBlock.generated.h"

UCLASS()
class MYPROJECT_API ATrapBlock : public AActor//AInteractiveActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void OnInterationVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetDefaultMeshLocation();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* BlockMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		UBoxComponent* InteractionVolume;
private:
	FTimerHandle FuzeTimerHandle;
	FVector StartLocation;
	FRotator StartRotator;
	void SetSimulatePhysics();
};
