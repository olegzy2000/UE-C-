// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterSpawner.h"
#include "../Characters/GCAICharacter.h"
#include "Actors/Interactive/Interactive.h"
// Sets default values
AAICharacterSpawner::AAICharacterSpawner()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnerRoot"));
	SetRootComponent(SceneRoot);
}

// Called when the game starts or when spawned
void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnTrigger.GetInterface()) {
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}
	if (bIsSpawnOnStart) {
		SpawnAI();
	}
}

// Called every frame
void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromTrigger();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterSpawner::UnSubscribeFromTrigger()
{
	if (TriggerHandle.IsValid() && SpawnTrigger.GetInterface()) {
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}

void AAICharacterSpawner::SpawnAI() {
	if (!bCanSpawn || !IsValid(CharacterClass)) {
		return;
	}
	 
	AGCAICharacter* AICharacter = GetWorld()->SpawnActor<AGCAICharacter>(CharacterClass, GetTransform());
	if (!IsValid(AICharacter->Controller)) {
		AICharacter->SpawnDefaultController();
	}
	if (bDoOnce) {
		UnSubscribeFromTrigger();
		bCanSpawn = false;
	}
}

void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property->GetName()==GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner,SpawnTriggerActor)) {
		SpawnTrigger = SpawnTriggerActor;
		if (SpawnTrigger.GetInterface()) {
			if (!SpawnTrigger->HasOnInteractionCallback()) {
				SpawnTriggerActor = nullptr;
				SpawnTrigger = nullptr;
			}
		}
		else {
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}
	}
}
