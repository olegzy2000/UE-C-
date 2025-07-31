// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystemTypes.h"
/*
FSaveSubsystemArchive::FSaveSubsystemArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails)
{
	ArIsSaveGame = true;
	ArNoDelta = true;
}

FSaveDirectoryVisitor::FSaveDirectoryVisitor(TArray<int32>& InSaveIds)
{
}

bool FSaveDirectoryVisitor::Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
{
	if (bIsDirectory) {
		return true;
	}
	const FString FullFilePath(FilenameOrDirectory);
	FString DirectoryName;
	FString FileName;
	FString FileExtension;
	FPaths::Split(FullFilePath, DirectoryName, FileName, FileExtension);
	if (FileExtension == FileExtensionSave.ToString()) {
		const int32 SaveId = FCString::Atoi(*FileName);
		if (SaveId > 0) {
			SaveIds.AddUnique(SaveId);
		}
	}
	return true;
}
*/