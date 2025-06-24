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
	virtual void BeginDestroy() override;
	UWeaponInventoryItem();
	void SetEquipWeaponClass(TSubclassOf<AEquipableItem>& EquipWeaponClass);
	bool Consume(AGCBaseCharacter* ConsumeTarget) override;
	TSubclassOf<AEquipableItem> GetEquipWeaponClass() const;
protected:
	UPROPERTY()
	TSubclassOf<AEquipableItem> EquipWeaponClass;
};
