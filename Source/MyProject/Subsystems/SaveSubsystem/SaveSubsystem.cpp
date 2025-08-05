// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveData.h"
#include "SaveSubsystemTypes.h"
#include "SaveSubsystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "SaveSubsystemInterface.h"
#include "SaveSubsystemUtils.h"
#include "GameFramework/Character.h"
#include <Runtime/Core/Public/Serialization/ArchiveSaveCompressedProxy.h>
#include <Runtime/Core/Public/Serialization/ArchiveLoadCompressedProxy.h>

const FGameSaveData& USaveSubsystem::GetGameSaveData() const
{
	return GameSaveData;
}

void USaveSubsystem::SaveGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SaveGame(): %s"), *GetNameSafe(this));
	SerializeGame();
	WriteSaveToFile();
}

void USaveSubsystem::LoadLastGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s"), *GetNameSafe(this));
	if (SaveIds.Num() == 0) {
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s failed! No saves."), *GetNameSafe(this));
		return;
	}
	LoadGame(SaveIds.Num() - 1);
}

void USaveSubsystem::LoadGame(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadGame()"));
	if (!SaveIds.Contains(SaveId)) {
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed!"));
		return;
	}
	LoadSaveFromFile(SaveId);
	UGameplayStatics::OpenLevel(this, GameSaveData.LevelName);
}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Initialize(): %s"), *GetNameSafe(this));
	GameSaveData = FGameSaveData();

	SaveDirectoryName = FString::Printf(TEXT("%sSaveGames/"), *FPaths::ProjectSavedDir());

	FSaveDirectoryVisitor DirectoryVisitor(SaveIds);
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*SaveDirectoryName, DirectoryVisitor);
	SaveIds.Sort();

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USaveSubsystem::OnPostLoadMapWithWorld);
}

void USaveSubsystem::Deinitialize()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Deinitialize(): %s"), *GetNameSafe(this));
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	Super::Deinitialize();
}

void USaveSubsystem::SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeLevel(): %s , Level: %s "), *GetNameSafe(this), *GetNameSafe(Level));
	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level;
	TArray<FActorSaveData>& ActorsSaveData = LevelSaveData->ActorsSaveData;
	ActorsSaveData.Empty();
	for (AActor* Actor : Level->Actors) {
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>()) {
			continue;
		}
		FActorSaveData& ActorSaveData = ActorsSaveData[ActorsSaveData.AddUnique(FActorSaveData(Actor))];
		ActorSaveData.Transform = Actor->GetTransform();
		FMemoryWriter MemoryWriter(ActorSaveData.RawData, true);
		FSaveSubsystemArchive Archive(MemoryWriter, false);
		Actor->Serialize(Archive);
	}
}

void USaveSubsystem::DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel)
{
	//UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Level: %s "), *GetNameSafe(this), *GetNameSafe(Level));
	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level;
	if (LevelSaveData == nullptr) {
		USaveSubsystemUtils::BroadcastOnLevelDeserialized(Level);
		return;
	}
	TArray<AActor*>ActorsToNotify;
	TArray<FActorSaveData*> ActorsSaveData;
	ActorsSaveData.Reserve(LevelSaveData->ActorsSaveData.Num());
	for (FActorSaveData& ActorSaveData : LevelSaveData->ActorsSaveData) {
		ActorsSaveData.Add(&ActorSaveData);
	}
	for (TArray<AActor*>::TIterator ActorIterator = Level->Actors.CreateIterator(); ActorIterator; ++ActorIterator) {
		AActor* Actor = *ActorIterator;
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>()) {
			continue;
		}
		FActorSaveData* ActorSaveData = nullptr;
		for (TArray<FActorSaveData*>::TIterator ActorSaveDataIterator = ActorsSaveData.CreateIterator(); ActorSaveDataIterator;++ActorSaveDataIterator) {
			if ((*ActorSaveDataIterator)->Name == Actor->GetFName()) {
				ActorSaveData = *ActorSaveDataIterator;
				ActorSaveDataIterator.RemoveCurrent();
				break;
			}
		}
		if (ActorSaveData == nullptr) {
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, ActorSaveData not found!"));
			Actor->Destroy();
		}
		else {
			DeserializeActor(Actor, ActorSaveData);
			if (Actor->Implements<USaveSubsystemInterface>()) {
				ActorsToNotify.Add(Actor);
			}
		}
	}
	UWorld* const World = GetWorld();
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = Level;
	ActorSpawnParameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (FActorSaveData* ActorSaveData : ActorsSaveData) {
		//UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Spawn new actor with name: %s"), *GetNameSafe(this), *GetNameSafe(ActorSaveData));
		ActorSpawnParameters.Name = ActorSaveData->Name;

		BoolScopeWrapper OnActorSpawnedHook(bIgnoreOnActorSpawnedCallback, true);
		AActor* Actor = World->SpawnActor(ActorSaveData->Class.Get(), &ActorSaveData->Transform, ActorSpawnParameters);
		if (!IsValid(Actor)) {
			//UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Spawn new actor with name: %s"), *GetNameSafe(this), *GetNameSafe(ActorSaveData));
			continue;
		}
		ActorSaveData->Name = Actor->GetFName();
		DeserializeActor(Actor, ActorSaveData);
		if (Actor->Implements<USaveSubsystemInterface>()) {
			ActorsToNotify.Add(Actor);
		}
	}

	for (AActor* Actor : ActorsToNotify) {
		ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
	}
}

void USaveSubsystem::DeserializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeGame(): %s , Level: %s "), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance();
	FMemoryReader MemoryReader(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	GameInstance->Serialize(Archive);
	const UWorld* World = GetWorld();
	DeserializeLevel(World->PersistentLevel);
}
void USaveSubsystem::SerializeGame() {
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeGame(): %s"), *GetNameSafe(this));
	UGameInstance* GameInstance = GetGameInstance();
	GameSaveData.GameInstance = FObjectSaveData(GetGameInstance());
	FMemoryWriter MemoryWriter(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryWriter, false);
	GameInstance->Serialize(Archive);

	const UWorld* World = GetWorld();
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (World->IsPlayInEditor()) {
		LevelName = UWorld::RemovePIEPrefix(LevelName);
	}
	GameSaveData.LevelName = FName(LevelName);
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(PlayerCharacter)) {
		GameSaveData.StartTransform = PlayerCharacter->GetTransform();
	}
	SerializeLevel(World->PersistentLevel);
}
void USaveSubsystem::WriteSaveToFile()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::WriteSaveToFile(): %s"), *GetNameSafe(this));
	const int32 SaveId = GetNextSaveId();
	SaveIds.AddUnique(SaveId);
	TArray<uint8>SaveBytes;
	FMemoryWriter MemoryWriter(SaveBytes);
	FObjectAndNameAsStringProxyArchive WriterArchive(MemoryWriter, false);
	GameSaveData.Serialize(WriterArchive);
	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*GetSaveFilePath(SaveId));

	if (bUseCompressedSaves) {
		TArray<uint8> CompressedSaveBytes;
		FArchiveSaveCompressedProxy CompressedArchive(CompressedSaveBytes, NAME_Zlib);
		CompressedArchive << SaveBytes;
		CompressedArchive.Flush();
		*FileWriter << CompressedSaveBytes;
	}
	else {
		*FileWriter << SaveBytes;
	}

	FileWriter->Close();
	delete FileWriter;
}

void USaveSubsystem::LoadSaveFromFile(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadSaveFromFile(): %s"), *GetNameSafe(this));

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId));
	TArray<uint8> SaveBytes;
	if (bUseCompressedSaves) {
		TArray<uint8> CompressedSaveBytes;
		*FileReader << CompressedSaveBytes;
		FArchiveLoadCompressedProxy DecompressedArchive(CompressedSaveBytes, NAME_Zlib);
		DecompressedArchive << SaveBytes;
		DecompressedArchive.Flush();
	}
	else {
		*FileReader << SaveBytes;
	}
	FileReader->Close();
	delete FileReader;

	FMemoryReader MemoryReader(SaveBytes, true);
	FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true);
	GameSaveData = FGameSaveData();
	GameSaveData.Serialize(ReaderArchive);
}

void USaveSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): %s"), *GetNameSafe(this));
	DeserializeGame();
}

void USaveSubsystem::DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeActor(): %s"), *GetNameSafe(this));
	Actor->SetActorTransform(ActorSaveData->Transform);
	FMemoryReader MemoryReader(ActorSaveData->RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	Actor->Serialize(Archive);
}

FString USaveSubsystem::GetSaveFilePath(int32 SaveId) const
{
	return SaveDirectoryName/FString::Printf(TEXT("%i.save"),SaveId);
}

int32 USaveSubsystem::GetNextSaveId() const
{
	if (SaveIds.Num() == 0) {
		return 1;
	}
	return SaveIds[SaveIds.Num()-1]+1;
}

void USaveSubsystem::OnActorSpawned(AActor* SpawnedActor)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s"), *GetNameSafe(this));
	if (bIgnoreOnActorSpawnedCallback) {
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): Skipped for actor"));
		return;
	}
	if (!IsValid(SpawnedActor) || !SpawnedActor->Implements<USaveSubsystemInterface>()) {
		return;
	}
	ISaveSubsystemInterface::Execute_OnLevelDeserialized(SpawnedActor);
}
