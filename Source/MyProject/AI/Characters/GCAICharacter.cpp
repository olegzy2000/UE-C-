// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Characters/GCAICharacter.h"
#include "../Components/AIPatrollingComponent.h"
void AGCAICharacter::Mantle(bool bForce)
{
	Super::Mantle(bForce);
}
UAIPatrollingComponent* AGCAICharacter::GetPatrollingComponent() const
{
	return AIPatrollingComponent;
}

AGCAICharacter::AGCAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer){
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));	
}

UBehaviorTree* AGCAICharacter::GetUBehaviorTree() const {
	return BehaviorTree;
}