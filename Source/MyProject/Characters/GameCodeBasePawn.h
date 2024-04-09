// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../Components/MovementComponents/GCBasePawnMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameCodeBasePawn.generated.h"

UCLASS()
class MYPROJECT_API AGameCodeBasePawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
		class USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base pawn")
		float CollisionSphereRadius = 50.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UArrowComponent* ArrowComponent;
#endif

private:
	UFUNCTION()
	void OnBlendComplete();
	AActor* CurrentViewActor;
	float InputForward = 0.0f;
	float InputRight = 0.0f;

	
public:	
	AGameCodeBasePawn();
	UPROPERTY(VisibleAnywhere)
	class UPawnMovementComponent * MovementComponent;
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();
	UFUNCTION(BlueprintCallable,BlueprintPure)
		float GetInputForward();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetInputRight();
};
