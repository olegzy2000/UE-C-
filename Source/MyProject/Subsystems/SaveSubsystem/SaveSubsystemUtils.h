// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveSubsystemUtils.generated.h"

/**
 * 
 */
class ULevel;
UCLASS()
class MYPROJECT_API USaveSubsystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Save subsystem utils")
	static void BroadcastOnLevelDeserialized(ULevel* Level);

};
