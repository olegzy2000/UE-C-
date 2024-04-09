// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCodeBasePawnAnimInstance.h"
#include "../GameCodeBasePawn.h"
#include "../../Components/MovementComponents/GCBasePawnMovementComponent.h"

void UGameCodeBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSecond)
{
	Super::NativeUpdateAnimation(DeltaSecond);
	if (!CachedBasePawn.IsValid()) {
		return;
	}
	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();
	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();

}
void UGameCodeBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AGameCodeBasePawn>(),TEXT("UGameCodeBasePawnAnimInstance::NativeBeginPlay() only GameCodeBasePawn can work with UGameCodeBasePawnAnimInstance"));
	CachedBasePawn = StaticCast<AGameCodeBasePawn*>(TryGetPawnOwner());
}

