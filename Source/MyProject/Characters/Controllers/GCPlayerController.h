// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "../GCBaseCharacter.h"
#include "../../Widget/ProgressBarWidget.h"
#include "../../Widget/AmmoWidget.h"
#include "../../Widget/PlayerHUD.h"
#include "../../Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/WidgetComponent.h"
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
		virtual void BeginPlay() override;
protected:
	virtual void SetupInputComponent() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User interface | setting")
		TSubclassOf<class UUserWidget> UserInterface;
	UPROPERTY()
		UPlayerHUD* PlayerHUD;

	

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
	void StartFireCustom();
	void StopFireCustom();
	void ChangeCrouchState();
	void EquipPrimaryItem();
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
	void StartAiming();
	void StopAiming();
	void Reload();
	void NexItem();
	void PreviousItem();
	void ChangeFireMode();
	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

public:
	virtual void SetPawn(APawn* InPawn) override;
	UPlayerHUD* GetPlayerHUD();
};
