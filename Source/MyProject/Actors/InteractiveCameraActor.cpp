// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

 AInteractiveCameraActor::AInteractiveCameraActor()
{
	 BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera interation volume"));
	 BoxComponent->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	 BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	 BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	 BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	 BoxComponent->SetupAttachment(RootComponent);
}

void AInteractiveCameraActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveCameraActor::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractiveCameraActor::OnEndOverlap);
}

void AInteractiveCameraActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FViewTargetTransitionParams TransitionToCamerParams;
	TransitionToCamerParams.BlendTime=TransitionToCameraTime;
	PlayerController->SetViewTarget(this, TransitionToCamerParams);
}

void AInteractiveCameraActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* pawn = PlayerController->GetPawn();
	FViewTargetTransitionParams TransitionToPawnParams;
	TransitionToPawnParams.BlendTime = TransitionToPlayerTime;
	TransitionToPawnParams.bLockOutgoing = true;
	PlayerController->SetViewTarget(pawn, TransitionToPawnParams);
}
