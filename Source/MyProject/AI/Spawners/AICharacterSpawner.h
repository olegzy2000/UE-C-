// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacterSpawner.generated.h"
class AGCAICharacter;
class IInteractable;
UCLASS()
class MYPROJECT_API AAICharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAICharacterSpawner();
	UFUNCTION()
	void SpawnAI();
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	TSubclassOf<AGCAICharacter>CharacterClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bIsSpawnOnStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bDoOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	AActor* SpawnTriggerActor;

private:
	bool bCanSpawn = true;
	void UnSubscribeFromTrigger();
	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggerHandle;


};
