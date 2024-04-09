// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "../GCBaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "CoreMinimal.h"
#include "GCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void SetupInputComponent() override;
private:
	UPROPERTY()
	TSoftObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void Mantle();
	void Jump();
	void Slide();
	void ChangeCrouchState();
	void ChangeProneState();
	void StartSprint();
	void StopSprint();
	void SwitchCameraPosition();
	void SwimRight(float Value);
	void SwimForward(float Value);
	void SwimUp(float Value);
	void ClimbLadderUp(float Value);
	void InteractionWithLadder();
	void InteractionWithZipline();
	void RunWall();
public:
	virtual void SetPawn(APawn* InPawn) override;
};
