// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapBlock.h"
#include <MyProject/GameCodeTypes.h>
// Sets default values
ATrapBlock::ATrapBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Block root"));
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	BlockMesh->SetupAttachment(RootComponent);
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(BlockMesh);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInterationVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ATrapBlock::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ATrapBlock::OnInterationVolumeBeginOverlap);
	BlockMesh->SetSimulatePhysics(false);
	StartLocation = BlockMesh->GetComponentLocation();
	StartRotator = BlockMesh->GetComponentRotation();
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



void ATrapBlock::OnInterationVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BlockMesh->SetSimulatePhysics(true);
	GetWorld()->GetTimerManager().SetTimer(
		FuzeTimerHandle, 
		this, 
		&ATrapBlock::SetDefaultMeshLocation,
		5.0f, 
		false); 
}
void ATrapBlock::SetDefaultMeshLocation() {
	BlockMesh->SetSimulatePhysics(false);
	BlockMesh->SetRelativeLocation(StartLocation);
	BlockMesh->SetRelativeRotation(StartRotator);
}
