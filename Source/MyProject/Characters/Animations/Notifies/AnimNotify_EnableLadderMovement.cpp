// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableLadderMovement.h"

#include "Characters/GCBaseCharacter.h"
#include "Components/MovementComponents/GCBaseCharacterMovementComponent.h"

void UAnimNotify_EnableLadderMovement::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation
)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(BaseCharacter))
	{
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent =
		BaseCharacter->GetBaseCharacterMovementComponent();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	if (!MovementComponent->IsOnLadder())
	{
		return;
	}

	MovementComponent->FinishLadderAttach();
	MovementComponent->UnblockMovement(EMovementBlockReason::LadderAttach);

	UE_LOG(LogTemp, Warning, TEXT(
		"AnimNotify_EnableLadderMovement: %s unblocked ladder movement"
	),
		*BaseCharacter->GetName());
}