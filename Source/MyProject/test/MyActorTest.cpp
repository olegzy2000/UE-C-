// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorTest.h"

// Sets default values
AMyActorTest::AMyActorTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Test root"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Test Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyActorTest::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyActorTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator CurrentRotation = Mesh->GetRelativeRotation();
	Mesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + 1, CurrentRotation.Roll));
}
