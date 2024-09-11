// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossWind.h"
#include <MyProject/Characters/GCBaseCharacter.h>

ACrossWind::ACrossWind()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void ACrossWind::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACrossWind::OnInterationVolumeOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ACrossWind::OnInterationVolumeOverlapEnd);
	generateWindForce();
}

void ACrossWind::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCanMove) {
		FVector newLocation=CurrentBaseCharacter->GetActorLocation();
		newLocation.X += CurrentWindForceX;
		newLocation.Y += CurrentWindForceY;
		CurrentBaseCharacter->SetActorLocation(newLocation);
	}
}

void ACrossWind::OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CurrentBaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(CurrentBaseCharacter))
	{
		return;
	}
	generateWindForce();
	bCanMove = true;
}

void ACrossWind::OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CurrentBaseCharacter = NULL;
	bCanMove = false;
}

void ACrossWind::generateWindForce()
{
	CurrentWindForceX= FMath::RandRange(0.0f, MaxWindForce);
	CurrentWindForceY= FMath::RandRange(0.0f, MaxWindForce);
	GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &ACrossWind::generateWindForce, 120.0f, false);
}

