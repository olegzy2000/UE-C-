// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamingSubsystemManager.h"
#include "StreamingSubsystemVolum.h"
DEFINE_LOG_CATEGORY(LogStreamingSubsystem);
void UStreamingSubsystemManager::OnLevelUnLoaded()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingLevelManager::OnLevelUnLoaded(): LevelName: %s"), *LevelName);
	if (!StreamingLevel.IsValid()) {
		return;
	}
	StreamingLevelState = StreamingLevel->GetCurrentState();
}
void UStreamingSubsystemManager::OnLevelLoaded()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingLevelManager::OnLevelLoaded(): LevelName: %s"), *LevelName);
	if (!StreamingLevel.IsValid()) {
		return;
	}
	StreamingLevelState = StreamingLevel->GetCurrentState();

}

void UStreamingSubsystemManager::LoadLevel()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingLevelManager::LoadLevel(): LevelName: %s"), *LevelName);
	if (!StreamingLevel.IsValid()) {
		return;
	}
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = true;

}

void UStreamingSubsystemManager::UnLoadLevel()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingLevelManager::UnLoadLevel(): LevelName: %s"), *LevelName);
	if (!StreamingLevel.IsValid()) {
		return;
	}
	StreamingLevel->SetShouldBeLoaded(false);
	StreamingLevel->SetShouldBeVisible(false);
	StreamingLevel->bShouldBlockOnLoad = true;
}

void UStreamingSubsystemManager::Initialize(ULevelStreaming* InStreamingLevel, const FString& InLevelName)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingLevelManager::Initialize(): StreamingLevel: %s , LevelName: %s"), *GetNameSafe(InStreamingLevel), *InLevelName);
	if (!IsValid(InStreamingLevel)) {
		return;
	}
	LevelName = InLevelName;
	StreamingLevel = InStreamingLevel;
	StreamingLevelState = StreamingLevel->GetCurrentState();

	StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &UStreamingSubsystemManager::OnLevelLoaded);
	StreamingLevel->OnLevelHidden.AddUniqueDynamic(this, &UStreamingSubsystemManager::OnLevelUnLoaded);
}

void UStreamingSubsystemManager::Deinitialize()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemManager::Deinitialize(): StreamingLevel: %s, LevelName %s"), *GetNameSafe(StreamingLevel.Get()), *LevelName);
	if (StreamingLevel.IsValid()) {
		StreamingLevel->OnLevelShown.RemoveDynamic(this, &UStreamingSubsystemManager::OnLevelLoaded);
		StreamingLevel->OnLevelHidden.RemoveDynamic(this, &UStreamingSubsystemManager::OnLevelUnLoaded);
	}
	StreamingLevel.Reset();

}

void UStreamingSubsystemManager::AddLoadRequest(AStreamingSubsystemVolum* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemManager::AddLoadRequest(): SubsystemVolume: %s, LevelName %s"), *GetNameSafe(SubsystemVolume), *LevelName);
	if (LoadRequests.Contains(SubsystemVolume)) {
		return;
	}
	LoadRequests.AddUnique(SubsystemVolume);

	if (LoadRequests.Num() == 1 && UnLoadRequests.Num() == 0) {
		LoadLevel();
	}
}

void UStreamingSubsystemManager::RemoveLoadRequest(AStreamingSubsystemVolum* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemManager::RemoveLoadRequest(): SubsystemVolume: %s, LevelName %s"), *GetNameSafe(SubsystemVolume), *LevelName);
	if (!LoadRequests.Contains(SubsystemVolume)) {
		return;
	}
	LoadRequests.Remove(SubsystemVolume);

	if (LoadRequests.Num() == 0 && UnLoadRequests.Num() == 0) {
		UnLoadLevel();
	}
}

void UStreamingSubsystemManager::AddUnloadRequest(AStreamingSubsystemVolum* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemManager::AddUnloadRequest(): SubsystemVolume: %s, LevelName %s"), *GetNameSafe(SubsystemVolume), *LevelName);
	if (UnLoadRequests.Contains(SubsystemVolume)) {
		return;
	}
	UnLoadRequests.AddUnique(SubsystemVolume);

	if (UnLoadRequests.Num() == 1 && LoadRequests.Num() != 0) {
		UnLoadLevel();
	}
}

void UStreamingSubsystemManager::RemoveUnloadRequest(AStreamingSubsystemVolum* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemManager::RemoveUnloadRequest(): SubsystemVolume: %s, LevelName %s"), *GetNameSafe(SubsystemVolume), *LevelName);
	if (!UnLoadRequests.Contains(SubsystemVolume)) {
		return;
	}
	UnLoadRequests.Remove(SubsystemVolume);

	if (UnLoadRequests.Num() == 0 && LoadRequests.Num() != 0) {
		LoadLevel();
	}
}

ULevelStreaming::ECurrentState UStreamingSubsystemManager::GetStreamingLevelState() const
{
	return ULevelStreaming::ECurrentState();
}
