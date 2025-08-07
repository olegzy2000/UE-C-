// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "SaveData.generated.h"
/**
 * 
 */

USTRUCT()
struct FBaseSaveData 
{

	GENERATED_BODY()

public:
	FBaseSaveData();
	virtual ~FBaseSaveData(){}
	virtual bool Serialize(FArchive& Archive);
	friend FArchive& operator << (FArchive& Archive, FBaseSaveData& SaveData) 
	{
		SaveData.Serialize(Archive);
		return Archive;
	}
	FName Name;
};
FORCEINLINE bool operator == (const FBaseSaveData& First, const FBaseSaveData& Second) { return First.Name == Second.Name; };

USTRUCT()
struct FObjectSaveData : public FBaseSaveData
{

	GENERATED_BODY()

public:
	FObjectSaveData();
	FObjectSaveData(const UObject* Object);
	virtual bool Serialize(FArchive& Archive) override;
	TStrongObjectPtr<UClass> Class;
	TArray<uint8> RawData;
};

USTRUCT()
struct FActorSaveData : public FObjectSaveData
{

	GENERATED_BODY()

public:
	FActorSaveData();
	FActorSaveData(const AActor* Object);
	virtual bool Serialize(FArchive& Archive) override;
	FTransform Transform;
	TArray<FObjectSaveData> ComponentsSaveData;
};

USTRUCT()
struct FLevelSaveData : public FBaseSaveData
{

	GENERATED_BODY()

public:
	FLevelSaveData();
	FLevelSaveData(const FName& LevelName);
	virtual bool Serialize(FArchive& Archive) override;
	TArray<FActorSaveData> ActorsSaveData;
};


USTRUCT()
struct FGameSaveData : public FBaseSaveData
{

	GENERATED_BODY()

public:
	FGameSaveData();
	virtual bool Serialize(FArchive& Archive) override;
	FName LevelName;
	FLevelSaveData Level;
	FObjectSaveData GameInstance;
	FTransform StartTransform;
	bool bIsSerialized;
};