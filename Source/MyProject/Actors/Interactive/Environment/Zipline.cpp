// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include <MyProject/GameCodeTypes.h>
AZipline::AZipline() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);

	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);

	ZiplineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZiplineMesh"));
	ZiplineMeshComponent->SetCollisionProfileName(CollisionProfilePawnInterationVolume);
	ZiplineMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInterationVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);

	ZiplineLedgeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZiplineLedge"));

}
void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!IsValid(RightRailMeshComponent) || !IsValid(LeftRailMeshComponent) ||
		!IsValid(ZiplineMeshComponent) || !IsValid(ZiplineMeshComponent->GetStaticMesh()))
	{
		return;
	}

	FVector StartLocation = RightRailMeshComponent->GetSocketLocation(FName("AttachPoint"));
	FVector EndLocation = LeftRailMeshComponent->GetSocketLocation(FName("AttachPoint"));

	float Distance = FVector::Dist(StartLocation, EndLocation);
	FVector MidPoint = (StartLocation + EndLocation) * 0.5f;
	FVector Direction = (EndLocation - StartLocation).GetSafeNormal();

	FRotator CableRotation = Direction.Rotation();
	CableRotation.Pitch -= 90.0f; 

	float MeshHeight = ZiplineMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z;
	float ScaleZ = (MeshHeight > 0.0f) ? (Distance / MeshHeight) : 1.0f;

	ZiplineMeshComponent->SetWorldLocation(MidPoint);
	ZiplineMeshComponent->SetWorldRotation(CableRotation);
	ZiplineMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, ScaleZ));

	if (IsValid(InteractionVolume))
	{
		UCapsuleComponent* Capsule = GetZiplineInteractionCapsule();
		if (Capsule)
		{
			Capsule->SetCapsuleHalfHeight(Distance * 0.5f);
			Capsule->SetWorldLocation(MidPoint);
			Capsule->SetWorldRotation(CableRotation);
		}
	}
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
}

float AZipline::GetZiplineLenght()const
{
	return ZiplineLength;
}


UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

void AZipline::OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	Super::OnInterationVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
}

void AZipline::OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInterationVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
}

UAnimMontage* AZipline::GetAttachFromTopAnimMontage() const
{
	return AttachFromTopAnimMontage;
}

FVector AZipline::GetAnimMontageStartingLocation() const
{
	return FVector::ZeroVector;
}

UStaticMeshComponent* AZipline::GetRightRailMeshComponent() const
{
	return RightRailMeshComponent;
}

UStaticMeshComponent* AZipline::GetLeftRailMeshComponent() const
{
	return LeftRailMeshComponent;
}
UStaticMeshComponent* AZipline::GetZiplineMeshComponent() const
{
	return ZiplineMeshComponent;
}

FVector AZipline::GetEndLocationMove()
{
	return EndLocationMove;
}

void AZipline::SetEndLocationMove(FVector EndLocation)
{
	EndLocationMove = EndLocation;
}



