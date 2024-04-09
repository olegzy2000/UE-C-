// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"
#include "Zipline.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
public:
	AZipline();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay()override;
	float GetZiplineLenght() const;
	UAnimMontage* GetAttachFromTopAnimMontage() const;
	FVector GetAnimMontageStartingLocation() const;
	UStaticMeshComponent* GetRightRailMeshComponent() const;
	UStaticMeshComponent* GetLeftRailMeshComponent() const;
	UStaticMeshComponent* GetZiplineMeshComponent() const;
	FVector GetEndLocationMove();
	void SetEndLocationMove(FVector EndLocation);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
		float ZiplineLength = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
		UAnimMontage* AttachFromTopAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
		FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline height")
		float ZiplineHeight = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* RightRailMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LeftRailMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ZiplineMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ZiplineLedgeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USplineComponent* ZiplineComponent;
	
	UCapsuleComponent* GetZiplineInteractionCapsule() const;

	virtual void OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	
public:
	FVector EndLocationMove;
};
