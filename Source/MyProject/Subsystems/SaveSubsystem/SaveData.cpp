// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveData.h"
FBaseSaveData::FBaseSaveData() {

}
bool FBaseSaveData::Serialize(FArchive& Archive) {
	Archive << Name;
	return true;
}

FObjectSaveData::FObjectSaveData() {

}
FObjectSaveData::FObjectSaveData(const UObject* Object) :Class(nullptr) {
	if (IsValid(Object)) {
		Name = FName(Object->GetName());
		Class = TStrongObjectPtr<UClass>(Object->GetClass());
	}
}

bool FObjectSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	UClass* ClassPtr = Class.Get();
	Archive << ClassPtr;
	if (Archive.IsLoading() && IsValid(ClassPtr)) {
		Class = TStrongObjectPtr<UClass>(ClassPtr);
	}
	Archive << RawData;
	return true;
}

FActorSaveData::FActorSaveData(const AActor* Actor)
	:Super(Actor)
	,Transform(FTransform::Identity)
{

}

bool FActorSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << Transform;
	Archive << ComponentsSaveData;
	return true;
}
FActorSaveData::FActorSaveData()
	:Transform(FTransform::Identity)
{

}
FLevelSaveData::FLevelSaveData() {

}
FLevelSaveData::FLevelSaveData(const FName& LevelName)
{
	Name = LevelName;
}

bool FLevelSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << ActorsSaveData;
	return true;
}

bool FGameSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << LevelName;
	Archive << Level;
	Archive << GameInstance;
	Archive << StartTransform;
	bIsSerialized = true;
	return true;
}
FGameSaveData::FGameSaveData()
	: Level(FName(TEXT("Persistent")))
	, StartTransform(FTransform::Identity)
	, bIsSerialized(false)
{

}
