// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnableLadderMovement.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAnimNotify_EnableLadderMovement : public UAnimNotify
{
	GENERATED_BODY()
public: 
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

};
