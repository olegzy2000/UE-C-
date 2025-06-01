// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlatform.h"
#include "TimerManager.h"
#include "../Characters/GCBaseCharacter.h"
// Sets default values
ABasePlatform::ABasePlatform() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);

}
// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();
	if (IsValid(PlatormInvocator)) {
		//PlatormInvocator.AddUObject(this, &ABasePlatform::StartMovingPlatform);
	}
		InitTimeline();
	if (PlatformBehavior == EPlatformBehavior::Loop) {
		StartMovingPlatform();
	}
}


void ABasePlatform::PlatformTimeluneUpdate(float Alpha)
{
	const FVector PlatformTagretLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	if (PlatformBehavior== EPlatformBehavior::OnDemand && PlatformTagretLocation == EndLocation) {
		PlatformTimeline.Reverse();
		//StopMovingPlatform();
	}
	PlatformMesh->SetRelativeLocation(PlatformTagretLocation);
}

void ABasePlatform::StartMovingPlatform()
{
	if(!PlatformTimeline.IsPlaying() && !PlatformTimeline.IsReversing())
	PlatformTimeline.PlayFromStart();
}



void ABasePlatform::StopMovingPlatform()
{
	PlatformTimeline.Stop();
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}
FVector ABasePlatform::GetDeltaMoving()
{
	return DeltaMoving;
}


void ABasePlatform::InitTimeline()
{
	InitCurveTimeLine();
	FOnTimelineFloatStatic PlatformMovementTimeLineUpdate;
	PlatformMovementTimeLineUpdate.BindUObject(this, &ABasePlatform::PlatformTimeluneUpdate);
	PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimeLineUpdate);
	if(PlatformBehavior == EPlatformBehavior::OnDemand)
	PlatformTimeline.SetLooping(false);
	else
	PlatformTimeline.SetLooping(true);
	//PlatformTimeline.Play();
}
void ABasePlatform::InitCurveTimeLine()
{
	TimelineCurve = NewObject<UCurveFloat>();
	FKeyHandle KeyHandleForProgressBar = TimelineCurve->FloatCurve.AddKey(0.f, 0.f);
	TimelineCurve->FloatCurve.AddKey(TimeToStop, 1.0f);
	TimelineCurve->FloatCurve.AddKey(TimeToStop*2, 0.0f);
	TimelineCurve->FloatCurve.SetKeyInterpMode(KeyHandleForProgressBar, ERichCurveInterpMode::RCIM_Linear, true);
}

