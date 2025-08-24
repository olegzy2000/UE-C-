// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/LevelStreaming.h"
#include "StreamingSubsystemManager.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(LogStreamingSubsystem, Log, All);
class AStreamingSubsystemVolum;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UStreamingSubsystemManager : public UObject
{
	GENERATED_BODY()
public:
	void Initialize(ULevelStreaming* InStreamingLevel, const FString& InLevelName);
	void Deinitialize();
	void AddLoadRequest(AStreamingSubsystemVolum* SubsystemVolume);
	void RemoveLoadRequest(AStreamingSubsystemVolum* SubsystemVolume);

	void AddUnloadRequest(AStreamingSubsystemVolum* SubsystemVolume);
	void RemoveUnloadRequest(AStreamingSubsystemVolum* SubsystemVolume);

	ULevelStreaming::ECurrentState GetStreamingLevelState() const;
private:
	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void OnLevelUnLoaded();

	void LoadLevel();
	void UnLoadLevel();


	FString LevelName;
	ULevelStreaming::ECurrentState StreamingLevelState = ULevelStreaming::ECurrentState::Unloaded;
	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolum>> LoadRequests;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolum>> UnLoadRequests;
};
