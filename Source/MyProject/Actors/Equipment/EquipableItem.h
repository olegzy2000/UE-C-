// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCodeTypes.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

UCLASS(Abstract,NotBlueprintable)
class MYPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

  protected:
	  UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Equipable item")
	  EEquipableItemType EquipableItemType=EEquipableItemType::None;
  public:
	  EEquipableItemType GetItemType();
};
