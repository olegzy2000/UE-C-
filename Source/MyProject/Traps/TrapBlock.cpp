// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapBlock.h"
#include <MyProject/GameCodeTypes.h>
ATrapBlock::ATrapBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Block root"));
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	BlockMesh->SetupAttachment(RootComponent);
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(BlockMesh);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInterationVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}
void ATrapBlock::BeginPlay()
{
	Super::BeginPlay();
	if (TrapType == ETrapType::Fall) {
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ATrapBlock::OnInterationVolumeBeginOverlapForFallType);
		BlockMesh->SetSimulatePhysics(false);
		StartLocation = BlockMesh->GetComponentLocation();
		StartRotator = BlockMesh->GetComponentRotation();
	}
	else if(TrapType == ETrapType::Hit) {
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ATrapBlock::OnInterationVolumeBeginOverlapForHitType);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ATrapBlock::OnInterationVolumeEndOverlapForHitType);
	}
}
void ATrapBlock::OnConstruction(const FTransform& Transform)
{
	InteractionVolume->SetRelativeLocation(StartLocation);
	FVector Size = BlockMesh->GetStaticMesh()->GetBoundingBox().GetSize();
	InteractionVolume->SetBoxExtent(FVector(Size.X/2,Size.Y/2,Size.Z+5.0f));
}

void ATrapBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void ATrapBlock::OnInterationVolumeBeginOverlapForFallType(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GetWorld()->GetTimerManager().SetTimer(
		FuzeTimerHandle,
		this,
		&ATrapBlock::SetSimulatePhysics,
		2.0f,
		false);
	BlockMesh->SetMaterial(0, ActiveFallMaterial);
}
void ATrapBlock::OnInterationVolumeBeginOverlapForHitType(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BlockMesh->SetMaterial(0, ActiveHitMaterial);
	CurrentBaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(CurrentBaseCharacter))
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(
		FuzeTimerHandle,
		this,
		&ATrapBlock::TakeDamageToCharacter,
		1.0f,
		false);
}
void ATrapBlock::OnInterationVolumeEndOverlapForHitType(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CurrentBaseCharacter = NULL;
	BlockMesh->SetMaterial(0, DefaultMaterial);
}
void ATrapBlock::SetDefaultMeshLocation() {
	BlockMesh->SetSimulatePhysics(false);
	BlockMesh->SetRelativeLocation(StartLocation);
	BlockMesh->SetRelativeRotation(StartRotator);
	BlockMesh->SetMaterial(0, DefaultMaterial);
}

void ATrapBlock::SetSimulatePhysics()
{
	BlockMesh->SetSimulatePhysics(true);
	GetWorld()->GetTimerManager().SetTimer(
		FuzeTimerHandle,
		this,
		&ATrapBlock::SetDefaultMeshLocation,
		5.0f,
		false);
}

void ATrapBlock::TakeDamageToCharacter()
{
	if (IsValid(CurrentBaseCharacter)) {
		CurrentBaseCharacter->TakeDamage(20.0f, FDamageEvent(), CurrentBaseCharacter->GetController(), this);
		GetWorld()->GetTimerManager().SetTimer(
			FuzeTimerHandle,
			this,
			&ATrapBlock::TakeDamageToCharacter,
			5.0f,
			false);
	}
}
