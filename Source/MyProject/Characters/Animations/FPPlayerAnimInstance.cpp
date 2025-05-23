// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "../FPPlayerCharacter.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(GetOwningActor()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay() only FPPlayer"))
	CachedFirstPersonCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());
}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedFirstPersonCharacterOwner.IsValid()) {
		return;
	}
	APlayerController* Controller = CachedFirstPersonCharacterOwner->GetController<APlayerController>();
	if (IsValid(Controller)) {
		PlayerCameraPitchAngle = Controller->GetControlRotation().Pitch;
	}
}
