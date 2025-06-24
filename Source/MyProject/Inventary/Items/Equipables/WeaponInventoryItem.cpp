// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInventoryItem.h"
#include "../../../Actors/Equipment/EquipableItem.h"
UWeaponInventoryItem::UWeaponInventoryItem() {
	bIsConsumable = true;
}
void UWeaponInventoryItem::BeginDestroy() {
	RemoveFromRoot();
}
void UWeaponInventoryItem::SetEquipWeaponClass(TSubclassOf<AEquipableItem>& EquipWeapon)
{
	EquipWeaponClass = EquipWeapon;
}

bool UWeaponInventoryItem::Consume(AGCBaseCharacter* ConsumeTarget)
{
	return false;
}

TSubclassOf<AEquipableItem> UWeaponInventoryItem::GetEquipWeaponClass() const
{
	return EquipWeaponClass;
}
