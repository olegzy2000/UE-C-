// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "TimerManager.h"
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnableRogdoll.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAnimNotify_EnableRogdoll : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation);
};
