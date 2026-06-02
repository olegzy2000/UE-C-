#include "MantleNavLinkProxy.h"
#include "../AI/Characters/GCAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AMantleNavLinkProxy::AMantleNavLinkProxy()
{
	bSmartLinkIsRelevant = true;
}

void AMantleNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	bSmartLinkIsRelevant = true;
	SetSmartLinkEnabled(true);

	OnSmartLinkReached.RemoveDynamic(this, &AMantleNavLinkProxy::HandleSmartLinkReached);
	OnSmartLinkReached.AddDynamic(this, &AMantleNavLinkProxy::HandleSmartLinkReached);
}

void AMantleNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor,const FVector& DestinationPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("MantleNavLink reached by: %s Destination=%s"),
		*GetNameSafe(MovingActor),
		*DestinationPoint.ToString());

	AGCAICharacter* AICharacter = Cast<AGCAICharacter>(MovingActor);
	if (!IsValid(AICharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("MantleNavLink failed: MovingActor is not AGCAICharacter"));
		ResumePathFollowing(MovingActor);
		return;
	}

	CachedAICharacter = AICharacter;

	AICharacter->ConsumeMovementInputVector();

	RotateCharacterToSmartLinkDestination(AICharacter, DestinationPoint);

	UE_LOG(LogTemp, Warning, TEXT("MantleNavLink calling Mantle. AIForward=%s"),
		*AICharacter->GetActorForwardVector().ToString());

	AICharacter->Mantle(true);
}

void AMantleNavLinkProxy::RotateCharacterToSmartLinkDestination(AGCAICharacter* AICharacter,const FVector& DestinationPoint) const
{
	if (!IsValid(AICharacter))
	{
		return;
	}

	FVector Direction = DestinationPoint - AICharacter->GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("MantleNavLink rotate skipped: Direction is zero"));
		return;
	}

	const FRotator TargetRotation = Direction.ToOrientationRotator();

	AICharacter->SetActorRotation(TargetRotation);

	UE_LOG(LogTemp, Warning, TEXT("MantleNavLink rotated AI to: %s Direction=%s"),
		*TargetRotation.ToString(),
		*Direction.ToString());
}