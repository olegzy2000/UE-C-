// Fill out your copyright notice in the Description page of Project Settings.


#include "Teleport.h"
#include "Components/BoxComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
// Sets default values
ATeleport::ATeleport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportRoot"));
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ATeleport::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ATeleport::OnInterationVolumeOverlapBegin);
	
}

void ATeleport::ShowWinText()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("You Win)))"), true, FVector2D(10, 10));
		float realtimeSeconds = GetWorld()->GetTimeSeconds();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Time in game %f seconds"), realtimeSeconds));
	}
}

void ATeleport::OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OtherActor->SetActorLocation(GetTransform().TransformPosition(EndLocationCoordinate));
	if(bIsFinish)
	ShowWinText();
}

