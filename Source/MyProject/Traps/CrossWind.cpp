// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossWind.h"
#include <MyProject/Characters/GCBaseCharacter.h>

// Sets default values
ACrossWind::ACrossWind()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ACrossWind::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACrossWind::OnInterationVolumeOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ACrossWind::OnInterationVolumeOverlapEnd);
}

// Called every frame
void ACrossWind::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACrossWind::OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}
	BaseCharacter->GetMovementComponent()->Velocity += BaseCharacter->GetActorRightVector() * 600;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
}

void ACrossWind::OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

