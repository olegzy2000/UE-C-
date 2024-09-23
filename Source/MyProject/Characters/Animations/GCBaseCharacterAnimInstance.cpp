// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacterAnimInstance.h"
#include "../GCBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "../../Components/MovementComponents/GCBaseCharacterMovementComponent.h"
void UGCBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}
	UGCBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouched();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsProning = CharacterMovement->IsProning();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	bIsRunningOnWall = CharacterMovement->IsRunningOnWall();
	bIsSliding = CharacterMovement->IsSlide();
	if (bIsRunningOnWall) {
		bIsLeft = CharacterMovement->GetIsLeft();
	}
	if(bIsOnLadder)
	LadderSpeedRation = CharacterMovement->GetLadderSpeedRation();
	bIsOnZipline = CharacterMovement->IsOnZipline();
	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovement->Velocity,CachedBaseCharacter->GetActorRotation());//direction of moving character
	AimRotation = CachedBaseCharacter->GetBaseAimRotation();
	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItem = CharacterEquipment->GetCurrentEquippedWeaponType();
}
void UGCBaseCharacterAnimInstance::setLeftEffectorLocation(FVector NewEffectorLocation)
{
	LeftFootEffectorLocation = NewEffectorLocation;
}
void UGCBaseCharacterAnimInstance::setRightEffectorLocation(FVector NewEffectorLocation)
{
	RightFootEffectorLocation = NewEffectorLocation;
}
FVector UGCBaseCharacterAnimInstance::getRightEffectorLocation()
{
	return RightFootEffectorLocation;
}
FVector UGCBaseCharacterAnimInstance::getLeftEffectorLocation()
{
	return LeftFootEffectorLocation;
}
void UGCBaseCharacterAnimInstance::SetLeftFootAlpha(float NewAlpha)
{
	LeftFootAlpha = NewAlpha;
}
void UGCBaseCharacterAnimInstance::SetRightFootAlpha(float NewAlpha)
{
	RightFootAlpha = NewAlpha;
}
UGCBaseCharacterAnimInstance::UGCBaseCharacterAnimInstance()
{
	JoinTargetLeft = FVector(50.f, 217.50296f, -38.598057f);
	JoinTargetRight = FVector(50.f, 217.f, 38.f);
}
void UGCBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AGCBaseCharacter>(), TEXT("UGCBaseCharacterAnimInstance::NativeBeginPlay() UGCBaseCharacterAnimInstance can be used with AGCBaseCharacter "));
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(TryGetPawnOwner());
}
