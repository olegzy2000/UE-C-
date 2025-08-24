// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamingSubsystemVolum.h"
#include "Components/BoxComponent.h"
#include "StreamingSubsystemUtils.h"
#include "StreamingSubsystemManager.h"
#include "StreamingSubsystem.h"
#include "GameFramework/Character.h"
// Sets default values
AStreamingSubsystemVolum::AStreamingSubsystemVolum()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

}
// Called when the game starts or when spawned
void AStreamingSubsystemVolum::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolum::BeginPlay(): %s"), *GetNameSafe(this));
	StreamingSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStreamingSubsystem>();
	
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AStreamingSubsystemVolum::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AStreamingSubsystemVolum::OnOverlapEnd);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	UStreamingSubsystemUtils::CheckStreamingSubsystemVolumeOverlapCharacter(this);
}
const TSet<FString>& AStreamingSubsystemVolum::GetLevelsToLoad() const
{
	return LevelsToLoad;
}

const TSet<FString>& AStreamingSubsystemVolum::GetLevelsToUnload() const
{
	return LevelsToUnload;
}

void AStreamingSubsystemVolum::HandleCharacterOverlapBegin(ACharacter* Character)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolum::HandleCharacterOverlapBegin(): %s , Character: %s"), *GetNameSafe(this), *GetNameSafe(Character));
	if (!StreamingSubsystem.IsValid() || !StreamingSubsystem->CanUseSubsystem()) {
		return;
	}
	if (!IsValid(Character) || !Character->IsPlayerControlled()) {
		return;
	}
	if (OverlappedCharacter.IsValid()) {
		return;
	}

	OverlappedCharacter = Character;
	StreamingSubsystem->OnVolumeOverlapBegin(this);

}



void AStreamingSubsystemVolum::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolum::EndPlay(): %s"), *GetNameSafe(this));

	CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AStreamingSubsystemVolum::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AStreamingSubsystemVolum::OnOverlapEnd);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (OverlappedCharacter.IsValid()) {
		OverlappedCharacter.Reset();
		StreamingSubsystem->OnVolumeOverlapEnd(this);
	}
	StreamingSubsystem.Reset();
	Super::EndPlay(EndPlayReason);
}

void AStreamingSubsystemVolum::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolum::OnOverlapBegin(): %s, OtherActor: %s"), *GetNameSafe(this),*GetNameSafe(OtherActor));
	if (!IsValid(OtherActor)) {
		return;
	}

	HandleCharacterOverlapBegin(Cast<ACharacter>(OtherActor));
}

void AStreamingSubsystemVolum::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolum::OnOverlapEnd(): %s , OtherActor: %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));
	if (!StreamingSubsystem.IsValid() || !StreamingSubsystem->CanUseSubsystem()) {
		return;
	}
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!IsValid(Character) || !Character->IsPlayerControlled()) {
		return;
	}
	if (OverlappedCharacter != OtherActor) {
		return;
	}

	OverlappedCharacter.Reset();
	StreamingSubsystem->OnVolumeOverlapEnd(this);
}

