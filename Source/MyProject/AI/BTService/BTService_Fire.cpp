// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_Fire.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include <Characters/GCBaseCharacter.h>
#include "GameCodeTypes.h"
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>

UBTService_Fire::UBTService_Fire() {
	NodeName = "Fire";
}
void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard)) {
		return;
	}
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(AIController->GetPawn());
	if (!IsValid(Character)) {
		return;
	}
	const UCharacterEquipmentComponent* EquipmentComponent=Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeaponItem();
	if (!IsValid(RangeWeapon)) {
		return;
	}
	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget)) {
		Character->StopFire();
		return;
	}
	float DistanceSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSq > FMath::Square(MaxFireDistance)) {
		Character->StopFire();
		Blackboard->SetValueAsBool(BB_IsFiring, NULL);
		return;
	}
	if (!(RangeWeapon->IsReloading() || RangeWeapon->IsFiring())) {
		Character->StartFire();
		Blackboard->SetValueAsBool(BB_IsFiring, true);
	}
}
