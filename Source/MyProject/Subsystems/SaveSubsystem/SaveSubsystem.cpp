// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSubsystem.h"
#include "SaveSubsystemTypes.h"
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
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed! SaveId %d not found."), SaveId);
		return;
	}

	if (!LoadSaveFromFile(SaveId))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed to read save file for SaveId %d."), SaveId);
		return;
	}

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
	if (!IsValid(Level))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::SerializeLevel(): Failed. Level is not valid."));
		return;
	}

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
		TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData.ComponentsSaveData;
		ComponentsSaveData.Empty();
		for (UActorComponent* ActorComponent : Actor->GetComponents()) {
			if (IsValid(ActorComponent) && ActorComponent->Implements<USaveSubsystemInterface>()) {
				FObjectSaveData& ComponentSaveData = ComponentsSaveData[ComponentsSaveData.Emplace(ActorComponent)];
				FMemoryWriter MemoryWriter(ComponentSaveData.RawData, true);
				FSaveSubsystemArchive Archive(MemoryWriter, false);
				ActorComponent->Serialize(Archive);
			}
		}
		FMemoryWriter MemoryWriter(ActorSaveData.RawData, true);
		FSaveSubsystemArchive Archive(MemoryWriter, false);
		Actor->Serialize(Archive);

	}
}
void USaveSubsystem::DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Level: %s "), *GetNameSafe(this), *GetNameSafe(Level));
	if (!IsValid(Level))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeLevel(): Failed. Level is not valid."));
		return;
	}

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.Level;
	if (LevelSaveData == nullptr) {
		USaveSubsystemUtils::BroadcastOnLevelDeserialized(Level);
		return;
	}
	TArray<AActor*> ActorsToNotify;
	TArray<FActorSaveData*> ActorsSaveData;
	ActorsSaveData.Reserve(LevelSaveData->ActorsSaveData.Num());
	for (FActorSaveData& ActorSaveData : LevelSaveData->ActorsSaveData) {
		ActorsSaveData.Add(&ActorSaveData);
	}

	// Исправленный цикл - используем стандартный for вместо TIterator
	for (int32 i = 0; i < Level->Actors.Num(); ++i) {
		AActor* Actor = Level->Actors[i];
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>()) {
			continue;
		}
		FActorSaveData* ActorSaveData = nullptr;

		// Исправленный поиск
		for (int32 j = 0; j < ActorsSaveData.Num(); ++j) {
			if (ActorsSaveData[j]->Name == Actor->GetFName()) {
				ActorSaveData = ActorsSaveData[j];
				ActorsSaveData.RemoveAt(j);
				break;
			}
		}

		// Исправленный вызов UE_LOG - убраны лишние параметры
		if (ActorSaveData == nullptr) {
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, ActorSaveData not found!"), *GetNameSafe(this));
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
	if (!IsValid(World))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeLevel(): Failed. World is not valid."));
		return;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = Level;
	ActorSpawnParameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FActorSaveData* ActorSaveData : ActorsSaveData) {
		if (ActorSaveData == nullptr || !ActorSaveData->Class.IsValid())
		{
			UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeLevel(): Skipped actor spawn. Save data or class is not valid."));
			continue;
		}

		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());
		ActorSpawnParameters.Name = ActorSaveData->Name;

		BoolScopeWrapper OnActorSpawnedHook(bIgnoreOnActorSpawnedCallback, true);
		AActor* Actor = World->SpawnActor(ActorSaveData->Class.Get(), &ActorSaveData->Transform, ActorSpawnParameters);
		if (!IsValid(Actor)) {
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s , Failed to spawn actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());
			continue;
		}
		ActorSaveData->Name = Actor->GetFName();
		DeserializeActor(Actor, ActorSaveData);
		if (Actor->Implements<USaveSubsystemInterface>()) {
			ActorsToNotify.Add(Actor);
		}
	}

	for (AActor* Actor : ActorsToNotify) {
		NotifyActorsAndComponents(Actor);
	}
}

void USaveSubsystem::NotifyActorsAndComponents(AActor* Actor)
{
	if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
	{
		return;
	}

	ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
	for (UActorComponent* ActorComponent : Actor->GetComponents()) {
		if (IsValid(ActorComponent) && ActorComponent->Implements<USaveSubsystemInterface>()) {
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(ActorComponent);
		}
	}
}

void USaveSubsystem::DeserializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeGame(): %s  "), *GetNameSafe(this));
	if (GameSaveData.bIsSerialized) {
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeGame(): Failed. GameInstance is not valid."));
		return;
	}

	FMemoryReader MemoryReader(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	GameInstance->Serialize(Archive);

	UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(World->PersistentLevel))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeGame(): Failed. World or PersistentLevel is not valid."));
		return;
	}

	DeserializeLevel(World->PersistentLevel);
}
void USaveSubsystem::SerializeGame() {
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeGame(): %s"), *GetNameSafe(this));
	UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = GetWorld();
	if (!IsValid(GameInstance) || !IsValid(World) || !IsValid(World->PersistentLevel))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::SerializeGame(): Failed. GameInstance, World, or PersistentLevel is not valid."));
		return;
	}

	GameSaveData.GameInstance = FObjectSaveData(GetGameInstance());
	FMemoryWriter MemoryWriter(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryWriter, false);
	GameInstance->Serialize(Archive);

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

	IFileManager::Get().MakeDirectory(*SaveDirectoryName, true);

	const int32 SaveId = GetNextSaveId();
	TArray<uint8> SaveBytes;
	FMemoryWriter MemoryWriter(SaveBytes);
	FObjectAndNameAsStringProxyArchive WriterArchive(MemoryWriter, false);
	GameSaveData.Serialize(WriterArchive);

	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*GetSaveFilePath(SaveId));
	if (FileWriter == nullptr)
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::WriteSaveToFile(): Failed to create file writer for %s"), *GetSaveFilePath(SaveId));
		return;
	}

	if (bUseCompressedSaves) {
		TArray<uint8> CompressedSaveBytes;
		FArchiveSaveCompressedProxy CompressedArchive(CompressedSaveBytes, NAME_Zlib);
		CompressedArchive << SaveBytes;
		CompressedArchive.Flush();
		if (CompressedArchive.GetError())
		{
			UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::WriteSaveToFile(): Failed to compress save data."));
			FileWriter->Close();
			delete FileWriter;
			return;
		}

		*FileWriter << CompressedSaveBytes;
	}
	else {
		*FileWriter << SaveBytes;
	}

	FileWriter->Close();
	delete FileWriter;
	SaveIds.AddUnique(SaveId);
}

bool USaveSubsystem::LoadSaveFromFile(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadSaveFromFile(): %s"), *GetNameSafe(this));

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId));
	if (FileReader == nullptr)
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadSaveFromFile(): Failed to create file reader for %s"), *GetSaveFilePath(SaveId));
		return false;
	}

	TArray<uint8> SaveBytes;
	if (bUseCompressedSaves) {
		TArray<uint8> CompressedSaveBytes;
		*FileReader << CompressedSaveBytes;
		FArchiveLoadCompressedProxy DecompressedArchive(CompressedSaveBytes, NAME_Zlib);
		DecompressedArchive << SaveBytes;
		DecompressedArchive.Flush();
		if (DecompressedArchive.GetError())
		{
			UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadSaveFromFile(): Failed to decompress save data."));
			FileReader->Close();
			delete FileReader;
			return false;
		}
	}
	else {
		*FileReader << SaveBytes;
	}
	FileReader->Close();
	delete FileReader;

	if (SaveBytes.Num() == 0)
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadSaveFromFile(): Save file is empty."));
		return false;
	}

	FMemoryReader MemoryReader(SaveBytes, true);
	FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true);
	GameSaveData = FGameSaveData();
	GameSaveData.Serialize(ReaderArchive);

	if (MemoryReader.IsError())
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadSaveFromFile(): Failed to deserialize save data."));
		GameSaveData = FGameSaveData();
		return false;
	}

	return true;
}

void USaveSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): %s"), *GetNameSafe(this));
	if (!IsValid(LoadedWorld))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): LoadedWorld is not valid."));
		return;
	}

	DeserializeGame();
}

void USaveSubsystem::DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData)
{
	if (!IsValid(Actor) || ActorSaveData == nullptr)
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::DeserializeActor(): Failed. Actor or ActorSaveData is not valid."));
		return;
	}

	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeActor(): %s , Actor name: %s"), *GetNameSafe(this), *Actor->GetName());
	Actor->SetActorTransform(ActorSaveData->Transform);

	const TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData->ComponentsSaveData;
	for (UActorComponent* ActorComponent : Actor->GetComponents()) {
		if (IsValid(ActorComponent) && ActorComponent->Implements<USaveSubsystemInterface>()) {
			const FObjectSaveData* ComponentSaveData = ComponentsSaveData.FindByPredicate([=](const FObjectSaveData& SaveData) {
				return SaveData.Name == ActorComponent->GetFName(); });
			if (ComponentSaveData == nullptr)
			{
				UE_LOG(LogSaveSubsystem, Verbose, TEXT("USaveSubsystem::DeserializeActor(): Component save data not found. Actor: %s, Component: %s"), *GetNameSafe(Actor), *GetNameSafe(ActorComponent));
				continue;
			}

			FMemoryReader MemoryReader(ComponentSaveData->RawData, true);
			FSaveSubsystemArchive Archive(MemoryReader, false);
			ActorComponent->Serialize(Archive);
		}
	}

	FMemoryReader MemoryReader(ActorSaveData->RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	Actor->Serialize(Archive);
}

FString USaveSubsystem::GetSaveFilePath(int32 SaveId) const
{
	return SaveDirectoryName / FString::Printf(TEXT("%i.save"), SaveId);
}

int32 USaveSubsystem::GetNextSaveId() const
{
	if (SaveIds.Num() == 0) {
		return 1;
	}
	return SaveIds[SaveIds.Num() - 1] + 1;
}

void USaveSubsystem::OnActorSpawned(AActor* SpawnedActor)
{
	if (!IsValid(SpawnedActor))
	{
		return;
	}

	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s , Actor name %s"), *GetNameSafe(this), *SpawnedActor->GetName());
	if (bIgnoreOnActorSpawnedCallback) {
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): Skipped for actor"));
		return;
	}
	if (!IsValid(SpawnedActor) || !SpawnedActor->Implements<USaveSubsystemInterface>()) {
		return;
	}
	NotifyActorsAndComponents(SpawnedActor);
}
