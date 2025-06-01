// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Controllers/GCAICharacterController.h"
#include "AI/Characters/GCAICharacter.h"
#include "AI/Components/AIPatrollingComponent.h"
#include<Perception/AISense_Sight.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "GameCodeTypes.h"
void AGCAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedAICharacter.IsValid()) {
		return;
	}
	TryMoveNextTarget();
}
void AGCAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess()) {
		return;
	}
	TryMoveNextTarget();
}
void AGCAICharacterController::TryMoveNextTarget()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor)) {
		if (IsValid(Blackboard)) {
			Blackboard->SetValueAsObject(BB_CurrentTarget, ClosestActor);
			SetFocus(ClosestActor,EAIFocusPriority::Gameplay);
		}
		//if (!IsTargetReached(ClosestActor->GetActorLocation())) {
		//	bIsPatrolling = false;
		//	MoveToActor(ClosestActor);
		//}
		return;
	}
	else if(PatrollingComponent->CanPatrol()) {
		FVector WayPoint;
		if (bIsPatrolling) {
			WayPoint = PatrollingComponent->SelectNextWayPoint();
		}
		else {
			WayPoint = PatrollingComponent->GetClosestWayPoint();
		}
		if (IsValid(Blackboard)) {
			Blackboard->SetValueAsVector(BB_NextLocation, WayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
			ClearFocus(EAIFocusPriority::Gameplay);
		}
		//if (!IsTargetReached(WayPoint)) {
		//	MoveToLocation(WayPoint);
		//}
		bIsPatrolling = true;
	}
}
bool AGCAICharacterController::IsTargetReached(FVector TargetLocation)
{
	return (TargetLocation-CachedAICharacter->GetActorLocation()).SizeSquared()<=FMath::Square(TargetReachRadius);
}
void AGCAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	//SetupPatrollingComponent();
}
void AGCAICharacterController::SetupPatrollingComponent()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	if (PatrollingComponent->CanPatrol()) {
		FVector ClosestWayPoint = PatrollingComponent->GetClosestWayPoint();
		//MoveToLocation(ClosestWayPoint);
		if (IsValid(Blackboard)) {
			Blackboard->SetValueAsVector(BB_NextLocation, ClosestWayPoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}
		bIsPatrolling = true;
	}
}
void AGCAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn)) {
		checkf(InPawn->IsA<AGCAICharacter>(), TEXT("AGCAICharacterController::SetPawn can possesed only by AGCAICharacter"));
		CachedAICharacter = StaticCast<AGCAICharacter*>(InPawn);
		RunBehaviorTree(CachedAICharacter->GetUBehaviorTree());
		SetupPatrollingComponent();
	}
	else {
		CachedAICharacter = nullptr;
	}
}
