// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerController.h"



void AGCPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->MoveForward(Value);
	}
}
void AGCPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->MoveRight(Value);
	}
}
void AGCPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Turn(Value);
	}
}
void AGCPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->LookUp(Value);
	}
}
void AGCPlayerController::TurnAtRate(float Value)
{

	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->TurnAtRate(Value);
	}
}
void AGCPlayerController::LookUpAtRate(float Value)
{

	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}
void AGCPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Mantle(false);
	}
}
void AGCPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Jump();
	}
}
void AGCPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Slide();
	}
}
void AGCPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AGCPlayerController::ChangeProneState()
{
	if(CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ChangeProneState();
	}
}

void AGCPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StartSprint();
	}
}

void AGCPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StopSprint();
	}
}
void AGCPlayerController::SwitchCameraPosition()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwitchCameraPosition();
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AGCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AGCBaseCharacter>(InPawn);
}
void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AGCPlayerController::InteractionWithLadder()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->InteractionWithLadder();
	}
}
void AGCPlayerController::InteractionWithZipline()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->InteractionWithZipline();
	}
}
void AGCPlayerController::RunWall()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->TryToRunWall();
	}
}

void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AGCPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCPlayerController::LookUpAtRate);
	InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithZipline", IE_Pressed, this, &AGCPlayerController::InteractionWithZipline);
	InputComponent->BindAction("InteractWithLadder", IE_Pressed, this, &AGCPlayerController::InteractionWithLadder);
	InputComponent->BindAction("Mantle", IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("RunWall", IE_Pressed, this, &AGCPlayerController::RunWall);
	InputComponent->BindAction("Jump",IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Slide", IE_Released, this, &AGCPlayerController::Slide);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AGCPlayerController::StopSprint);
	InputComponent->BindAction("SwitchCameraPosition", IE_Released, this, &AGCPlayerController::SwitchCameraPosition);
	InputComponent->BindAction("Prone", IE_Released, this, &AGCPlayerController::ChangeProneState);

	InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
}