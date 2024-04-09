// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "../InteractiveActor.h"
#include "Ladder.generated.h"
/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
public:
	ALadder();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay()override;
	float GetLadderHeight() const;
	bool GetIsOnTop() const;
	UAnimMontage* GetAttachFromTopAnimMontage() const;
	FVector GetAnimMontageStartingLocation() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderHeight = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderWidth = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float StepsInterval = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float BottomStepsOffset = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		 UAnimMontage* AttachFromTopAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		FVector AttachFromTopAnimMontageInitialOffset=FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* RightRailMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LeftRailMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UInstancedStaticMeshComponent* StepsMeshComponent;
	UBoxComponent* GetLadderInteractionBox() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;
	virtual void OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	bool bIsOnTop;
};
