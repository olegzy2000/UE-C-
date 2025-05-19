// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/GCAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include <Characters/GCBaseCharacter.h>

AActor* AGCAIController::GetClosestSensedActor(TSubclassOf<UAISense> SensedClass)const
{
	if (!IsValid(GetPawn())) {
		return nullptr;
	}
	TArray<AActor*>SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SensedClass, SensedActors);
	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();
	for (AActor* SensedActor : SensedActors) {
		if (SensedActor != nullptr && SensedActor->IsA<AGCBaseCharacter>()) {
			AGCBaseCharacter* CurrentTargetCharacter = Cast<AGCBaseCharacter>(SensedActor);
			if (CurrentTargetCharacter->GetCharacterAttributesComponent()->GetHealth() > 0) {
				float CurretSquaredDistance = (PawnLocation - SensedActor->GetActorLocation()).SizeSquared();
				if (CurretSquaredDistance < MinSquaredDistance) {
					MinSquaredDistance = CurretSquaredDistance;
					ClosestActor = SensedActor;
				}
			}
		}
	}
	return ClosestActor;
	//CachedTurret->SetCurrentTarget(ClosestActor);
}
AGCAIController::AGCAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

