// Fill out your copyright notice in the Description page of Project Settings.


#include "ZiplineNavLinkProxy.h"
#include "MantleNavLinkProxy.h"
#include "../AI/Characters/GCAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AZiplineNavLinkProxy::AZiplineNavLinkProxy()
{
	bSmartLinkIsRelevant = true;
}

void AZiplineNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	bSmartLinkIsRelevant = true;
	SetSmartLinkEnabled(true);

	OnSmartLinkReached.RemoveDynamic(this, &AZiplineNavLinkProxy::HandleSmartLinkReached);
	OnSmartLinkReached.AddDynamic(this, &AZiplineNavLinkProxy::HandleSmartLinkReached);
}

void AZiplineNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("ZiplineNavLinkProxy reached by: %s Destination=%s"),
		*GetNameSafe(MovingActor),
		*DestinationPoint.ToString());

	AGCAICharacter* AICharacter = Cast<AGCAICharacter>(MovingActor);
	if (!IsValid(AICharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("ZiplineNavLinkProxy failed: MovingActor is not AGCAICharacter"));
		ResumePathFollowing(MovingActor);
		return;
	}

	AICharacter->InteractionWithZipline();
}