// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_Throw.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include <Characters/GCBaseCharacter.h>
#include "GameCodeTypes.h"
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>

UBTService_Throw::UBTService_Throw() {
	NodeName = "Throw";
}
void UBTService_Throw::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard)) {
		return;
	}
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(AIController->GetPawn());
	if (!IsValid(Character)) {
		return;
	}
	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	
	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget)) {
		return;
	}
	float DistanceSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSq > FMath::Square(MinThrowDistance) && !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle)) {
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("SetThrowTimer"), &OwnerComp);
		CurrentTimeToThrow=FMath::RandRange(5, 8);
		UE_LOG(LogTemp, Log, TEXT("new time to throw: %f"), CurrentTimeToThrow);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, CurrentTimeToThrow, false);
	}
}

void UBTService_Throw::SetThrowTimer(UBehaviorTreeComponent* OwnerComp)
{
	UE_LOG(LogTemp, Log, TEXT("throw function called"));
	if (IsValid(OwnerComp)) {
	AAIController* AIController = (*OwnerComp).GetAIOwner();
	UBlackboardComponent* Blackboard = (*OwnerComp).GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard)) {
		return;
	}
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(AIController->GetPawn());
	if (!IsValid(Character)) {
		return;
	}
	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget)) {
		return;
	}
	float DistanceSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSq > FMath::Square(MinThrowDistance)) {
		Character->EquipPrimaryItem();
	}
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}
