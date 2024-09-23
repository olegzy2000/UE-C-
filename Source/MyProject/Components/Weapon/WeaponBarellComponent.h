// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	void Shot();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attrubutes")
		float FiringRange = 5000.0f;
};
