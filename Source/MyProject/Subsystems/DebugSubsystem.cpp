// Fill out your copyright notice in the Description page of Project Settings.
#include "DebugSubsystem.h"

bool UDebugSubsystem::IsCategoryEnable(const FName& CategoryName) const {
	const bool* bIsEnabled = EnableDebugCategories.Find(CategoryName);
	return bIsEnabled != nullptr && *bIsEnabled;
}

void UDebugSubsystem::EnableDebugCategory(const FName& CategoryName, bool bIsEnabled)
{
	EnableDebugCategories.FindOrAdd(CategoryName);
	EnableDebugCategories[CategoryName] = bIsEnabled;
}
