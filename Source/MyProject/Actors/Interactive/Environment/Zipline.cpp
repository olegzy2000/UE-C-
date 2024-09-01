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
	FVector StartLocation = RightRailMeshComponent->GetSocketLocation(FName("AttachPoint"));
	FVector EndLocation = LeftRailMeshComponent->GetSocketLocation(FName("AttachPoint"));
	float Delta = sqrt(pow(StartLocation.X - EndLocation.X, 2) + pow(StartLocation.Y - EndLocation.Y, 2) + pow(StartLocation.Z - EndLocation.Z, 2));
	float Length = sqrt(pow(StartLocation.X - EndLocation.X, 2) + pow(StartLocation.Y - EndLocation.Y, 2) + pow(StartLocation.Z - EndLocation.Z, 2));
	if (IsValid(ZiplineMeshComponent) && IsValid(ZiplineMeshComponent->GetStaticMesh()) && IsValid(RightRailMeshComponent) && IsValid(LeftRailMeshComponent)) {
		float MeshHeight = ZiplineMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z;
		float X = (StartLocation.X + EndLocation.X) / 2;
		float Y = (StartLocation.Y + EndLocation.Y) / 2;
		float Z = (StartLocation.Z + EndLocation.Z) / 2;
		float DeltaX = -StartLocation.X + EndLocation.X;
		float DeltaY = -StartLocation.Y + EndLocation.Y;
		float DeltaZ = -StartLocation.Z + EndLocation.Z;
		FRotator CableRotation = ZiplineMeshComponent->GetRelativeRotation() ;
		CableRotation = (FVector(DeltaX, DeltaY, DeltaZ).ToOrientationRotator());
		CableRotation.Pitch -= 90;
		ZiplineMeshComponent->SetRelativeRotation(CableRotation);
		ZiplineMeshComponent->SetWorldLocation(FVector(X, Y, Z));
		ZiplineMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, Length / MeshHeight));
	}
	if (IsValid(InteractionVolume)) {
		GetZiplineInteractionCapsule()->SetCapsuleHalfHeight(Length/2);
		InteractionVolume->SetWorldLocation(ZiplineMeshComponent->GetComponentLocation());
		InteractionVolume->SetRelativeRotation(ZiplineMeshComponent->GetRelativeRotation());
	}
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AZipline::OnInterationVolumeOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AZipline::OnInterationVolumeOverlapEnd);
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
	// TO DO
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



