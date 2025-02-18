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
AAITurretController::AAITurretController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("TurretPerception"));
	//PerceptionComponent.
}
void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!CachedTurret.IsValid()) {
		return;
	}
	TArray<AActor*>SeenActors;
	//PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(),SeenActors);
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), SeenActors);
	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector TurretLocation = CachedTurret->GetActorLocation();
	for (AActor* SeenActor : SeenActors) {
		if (SeenActor != nullptr && SeenActor->IsA<AGCBaseCharacter>()) {
			AGCBaseCharacter* CurrentTargetCharacter = Cast<AGCBaseCharacter>(SeenActor);
			if (CurrentTargetCharacter->GetCharacterAttributesComponent()->GetHealth() > 0) {
				float CurretSquaredDistance = (TurretLocation - SeenActor->GetActorLocation()).SizeSquared();
				if (CurretSquaredDistance < MinSquaredDistance) {
					MinSquaredDistance = CurretSquaredDistance;
					ClosestActor = SeenActor;
				}
			}
		}
	}
	CachedTurret->SetCurrentTarget(ClosestActor);
}

void AAITurretController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}

	TArray<AActor*> DamageInstigators;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), DamageInstigators);
	for (AActor* DamageInstigator : DamageInstigators)
	{
		if (IsValid(DamageInstigator))
		{
			CachedTurret->SetCurrentTarget(DamageInstigator);
			return;
		}
	}

	//AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	//CachedTurret->CurrentTarget = ClosestActor;
	//CachedTurret->OnCurrentTargetSet();
}

void AAITurretController::BeginPlay()
{
	Super::BeginPlay();
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAITurretController::OnPerceptionUpdated);
}
