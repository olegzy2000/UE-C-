// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StreamingSubsystem.generated.h"
class UStreamingSubsystemManager;
class AStreamingSubsystemVolum;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UStreamingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual UWorld* GetWorld() const override;
	void OnVolumeOverlapBegin(AStreamingSubsystemVolum* SubsystemVolume);
	void OnVolumeOverlapEnd(AStreamingSubsystemVolum* SubsystemVolume);
	bool CanUseSubsystem() const;
private:
	void CreateStreamingLevelManagers(UWorld* World);
	void RemoveStreammingLevelManagers();
	void OnPreLoadMap(const FString& MapName);
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
	bool FindStreamingLevelManager(const FString& LevelName, UStreamingSubsystemManager*& LevelManager);
	UPROPERTY(Transient)
	TMap<FString, UStreamingSubsystemManager*> StreamingLevelManagers;
};
