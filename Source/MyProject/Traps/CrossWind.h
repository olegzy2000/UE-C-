// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TimerManager.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrossWind.generated.h"

UCLASS()
class MYPROJECT_API ACrossWind : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrossWind();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		 void OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		 void OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wind volume")
		class UPrimitiveComponent* InteractionVolume;
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MaxWindForce = 2.0f;
private:
	class AGCBaseCharacter* CurrentBaseCharacter;
	bool bCanMove = false;
	float CurrentWindForceX;
	float CurrentWindForceY;
	void generateWindForce();
	FTimerHandle MyTimerHandle;
};
