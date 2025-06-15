// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventary/InventoryItem.h"
#include "WeaponInventoryItem.generated.h"

/**
 * 
 */
class AEquipableItem;
UCLASS()
class MYPROJECT_API UWeaponInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
public:
	UWeaponInventoryItem();
	void SetEquipWeaponClass(TSubclassOf<AEquipableItem>& EquipWeaponClass);
	bool Consume(AGCBaseCharacter* ConsumeTarget) override;

	TSubclassOf<AEquipableItem> GetEquipWeaponClass() const;
protected:
	TSubclassOf<AEquipableItem> EquipWeaponClass;
};
