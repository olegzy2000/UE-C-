// Fill out your copyright notice in the Description page of Project Settings.

#include "LadderNavLinkProxy.h"
#include "../AI/Characters/GCAICharacter.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Navigation/PathFollowingComponent.h"
#include "../Characters/Controllers/GCAICharacterController.h"
#include "Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "AIController.h"

ALadderNavLinkProxy::ALadderNavLinkProxy()
{
	PrimaryActorTick.bCanEverTick = true;
	bSmartLinkIsRelevant = true;
}

void ALadderNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	bSmartLinkIsRelevant = true;
	SetSmartLinkEnabled(true);
	OnSmartLinkReached.RemoveDynamic(this, &ALadderNavLinkProxy::HandleSmartLinkReached);
	OnSmartLinkReached.AddDynamic(this, &ALadderNavLinkProxy::HandleSmartLinkReached);
}

void ALadderNavLinkProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PendingAICharacter.IsValid())
	{
		ClimbTick(DeltaTime);
	}
}
void ALadderNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("LadderNavLink reached by: %s Destination=%s"),
		*GetNameSafe(MovingActor),
		*DestinationPoint.ToString());

	AGCAICharacter* AICharacter = Cast<AGCAICharacter>(MovingActor);
	if (!IsValid(AICharacter))
	{
		ResumePathFollowing(MovingActor);
		return;
	}

	if (PendingAICharacter.Get() == AICharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("LadderNavLink duplicate call for same AI ignored"));
		return;
	}

	if (!IsValid(TargetLadder))
	{
		UE_LOG(LogTemp, Error, TEXT("LadderNavLink failed: TargetLadder is not set"));
		ResumePathFollowing(MovingActor);
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent =
		AICharacter->GetBaseCharacterMovementComponent();

	if (!IsValid(MovementComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("LadderNavLink failed: MovementComponent is invalid"));
		ResumePathFollowing(MovingActor);
		return;
	}

	AICharacter->ConsumeMovementInputVector();
	MovementComponent->StopMovementImmediately();

	bClimbUp = DestinationPoint.Z > AICharacter->GetActorLocation().Z;

	UE_LOG(LogTemp, Warning, TEXT("LadderNavLink climb direction: %s CurrentZ=%.2f DestinationZ=%.2f"),
		bClimbUp ? TEXT("Up") : TEXT("Down"),
		AICharacter->GetActorLocation().Z,
		DestinationPoint.Z);

	PendingAICharacter = AICharacter;
	AICharacter->GetCharacterInteractionComponent()->RegisterInteractiveActor(TargetLadder);
	AICharacter->InteractionWithLadder();
}

void ALadderNavLinkProxy::ClimbTick(float DeltaTime)
{
	AGCAICharacter* AICharacter = PendingAICharacter.Get();
	if (!IsValid(AICharacter))
	{
		FinishLadderTraversal();
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent =
		AICharacter->GetBaseCharacterMovementComponent();

	if (!IsValid(MovementComponent))
	{
		FinishLadderTraversal();
		return;
	}

	if (MovementComponent->IsOnLadder())
	{
		const float InputValue = bClimbUp ? 1.0f : -1.0f;
		AICharacter->ClimbLadderUp(InputValue);
		return;
	}

	if (MovementComponent->IsMantling())
	{
		return;
	}

	FinishLadderTraversal();
}

void ALadderNavLinkProxy::FinishLadderTraversal()
{
	AGCAICharacter* AICharacter = PendingAICharacter.Get();
	PendingAICharacter.Reset();

	if (IsValid(AICharacter))
	{
		ResumePathFollowing(AICharacter);
		AICharacter->GetCharacterInteractionComponent()->UnRegisterInteractiveActor(TargetLadder);
	}
	if (AGCAICharacterController* AIController = Cast<AGCAICharacterController>(AICharacter->GetController()))
	{
		AIController->ContinuePatrolAfterTraversal();
	}
}