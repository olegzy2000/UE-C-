// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AITurretController.h"
#include<Perception/AISense_Sight.h>
#include<Perception/AIPerceptionComponent.h>
#include<Perception/AISense_Damage.h>
#include<Perception/AISenseEvent_Damage.h>
#include "../Turrent/Turret.h"
#include <Characters/GCBaseCharacter.h>
void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn)) {
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AAITurretController can possess only T"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else {
		CachedTurret = nullptr;
	}
}
void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid()) {
		return;
	}
	//TArray<AActor*>SeenActors;
	//PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(),SeenActors);
	//PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), SeenActors);
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	CachedTurret->SetCurrentTarget(ClosestActor);
}

void AAITurretController::BeginPlay()
{
	Super::BeginPlay();
}
