// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()
public :
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
		class USkeletalMeshComponent* FirstPersonMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
		class UCameraComponent* FirstPersonCameraComponent;
};
