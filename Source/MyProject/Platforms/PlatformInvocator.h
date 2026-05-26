// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvocator.generated.h"

UCLASS()
class MYPROJECT_API APlatformInvocator : public AActor
{
	GENERATED_BODY()
	
public:	
	APlatformInvocator();
	DECLARE_EVENT(APlatformInvocator, FInvocatorActivated);
	FInvocatorActivated InvocatorActivated;
protected:
	
public:	
	UFUNCTION(BlueprintCallable)
	void InvocePlatformToStart();

};
