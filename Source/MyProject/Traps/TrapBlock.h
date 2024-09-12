// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <MyProject/Characters/GCBaseCharacter.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Actors/Interactive/InteractiveActor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "TrapBlock.generated.h"
UENUM()
enum class ETrapType : uint8 {
	Fall = 0,
	Hit
};
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
	void OnInterationVolumeBeginOverlapForFallType(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInterationVolumeBeginOverlapForHitType(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInterationVolumeEndOverlapForHitType(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void SetDefaultMeshLocation();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETrapType TrapType= ETrapType::Fall;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BlockMesh;
	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;
	UPROPERTY(EditAnywhere)
	UMaterial* ActiveHitMaterial;
	UPROPERTY(EditAnywhere)
	UMaterial* ActiveFallMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UBoxComponent* InteractionVolume;
private:
	class AGCBaseCharacter* CurrentBaseCharacter;
	FTimerHandle FuzeTimerHandle;
	FVector StartLocation;
	FRotator StartRotator;
	void SetSimulatePhysics();
	void TakeDamageToCharacter();
};
