// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInteractionComponent.h"

#include "Characters/GCBaseCharacter.h"
#include "Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "GameFramework/PlayerController.h"

UCharacterInteractionComponent::UCharacterInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedBaseCharacter = Cast<AGCBaseCharacter>(GetOwner());
	if (!CachedBaseCharacter.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("UCharacterInteractionComponent::BeginPlay failed: owner is not AGCBaseCharacter"));
		return;
	}

	CachedBaseCharacterMovementComponent = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	if (!CachedBaseCharacterMovementComponent.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("UCharacterInteractionComponent::BeginPlay failed: movement component is not valid"));
	}

	CachedPlayerController = CachedBaseCharacter->GetController<APlayerController>();
}

APlayerController* UCharacterInteractionComponent::GetCachedPlayerController()
{
	if (!CachedPlayerController.IsValid() && CachedBaseCharacter.IsValid()) {
		CachedPlayerController = CachedBaseCharacter->GetController<APlayerController>();
	}

	return CachedPlayerController.Get();
}

void UCharacterInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TraceOfSight();
}

void UCharacterInteractionComponent::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	if (IsValid(InteractiveActor)) {
		AvailableInteractiveActors.AddUnique(InteractiveActor);
	}
}

void UCharacterInteractionComponent::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

void UCharacterInteractionComponent::Interact()
{
	if (LineOfSightObject.GetInterface()) {
		LineOfSightObject->Interact(CachedBaseCharacter.Get());
	}
}

void UCharacterInteractionComponent::ClimbLadderUp(float Value)
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent = CachedBaseCharacterMovementComponent.Get();
	if (IsValid(MovementComponent) && MovementComponent->IsOnLadder() && !FMath::IsNearlyZero(Value)) {
		FVector LadderUpVector = MovementComponent->GetCurrentLadder()->GetActorUpVector();
		CachedBaseCharacter->AddMovementInput(LadderUpVector, Value);
	}
}

void UCharacterInteractionComponent::InteractionWithLadder()
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent = CachedBaseCharacterMovementComponent.Get();
	if (!IsValid(MovementComponent)) {
		return;
	}

	if (MovementComponent->IsOnLadder()) {
		MovementComponent->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else {
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder)) {
			if (AvailableLadder->GetIsOnTop()) {
				CachedBaseCharacter->PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			MovementComponent->AttachToLadder(AvailableLadder);
		}
	}
}

void UCharacterInteractionComponent::InteractionWithZipline()
{
	if (!CachedBaseCharacter.IsValid()) {
		return;
	}

	UGCBaseCharacterMovementComponent* MovementComponent = CachedBaseCharacterMovementComponent.Get();
	if (!IsValid(MovementComponent)) {
		return;
	}

	if (MovementComponent->IsOnZipline()) {
		MovementComponent->DetachFromZipline(EDetachFromLadderMethod::JumpOff);
	}
	else {
		AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline)) {
			MovementComponent->AttachToZipline(AvailableZipline);
		}
	}
}

const ALadder* UCharacterInteractionComponent::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors) {
		if (IsValid(InteractiveActor) && InteractiveActor->IsA<ALadder>()) {
			Result = Cast<const ALadder>(InteractiveActor);
			break;
		}
	}
	return Result;
}

AZipline* UCharacterInteractionComponent::GetAvailableZipline() const
{
	AZipline* Result = nullptr;
	for (AInteractiveActor* InteractiveActor : AvailableInteractiveActors) {
		if (IsValid(InteractiveActor) && InteractiveActor->IsA<AZipline>()) {
			Result = Cast<AZipline>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void UCharacterInteractionComponent::TraceOfSight()
{
	if (!CachedBaseCharacter.IsValid() || !CachedBaseCharacter->IsPlayerControlled()) {
		return;
	}

	APlayerController* PlayerController = GetCachedPlayerController();
	if (!IsValid(PlayerController) || !IsValid(GetWorld())) {
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector ViewDirection = ViewRotation.Vector();
	const FVector TraceEnd = ViewLocation + ViewDirection * LineSightDistance;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);

	if (HitResult.GetActor() != nullptr) {
		LineOfSightObject = HitResult.GetActor();
		FName ActionName;
		if (LineOfSightObject.GetInterface()) {
			ActionName = LineOfSightObject->GetActionEventName();
		}
		else {
			ActionName = NAME_None;
		}
		OnInteractableObjectFound.ExecuteIfBound(ActionName);
	}
	else {
		LineOfSightObject = nullptr;
		OnInteractableObjectFound.ExecuteIfBound(NAME_None);
	}
}
