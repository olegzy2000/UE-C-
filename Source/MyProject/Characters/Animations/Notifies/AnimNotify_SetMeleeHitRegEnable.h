// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetMeleeHitRegEnable.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAnimNotify_SetMeleeHitRegEnable : public UAnimNotify
{
	GENERATED_BODY()
		virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* AnimSequence)override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee weapon")
		bool bIsHitRegistrationEnabled=false;

	
};
