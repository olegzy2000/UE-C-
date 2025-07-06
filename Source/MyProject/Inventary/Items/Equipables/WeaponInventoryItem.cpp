// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInventoryItem.h"
#include "../../../Actors/Equipment/EquipableItem.h"
UWeaponInventoryItem::UWeaponInventoryItem() {
	bIsConsumable = true;
}
void UWeaponInventoryItem::BeginDestroy() {
	if (IsRooted())
	{
		RemoveFromRoot();
	}
	Super::BeginDestroy();
}
void UWeaponInventoryItem::SetEquipWeaponClass(TSubclassOf<AEquipableItem>& EquipWeapon)
{
	EquipWeaponClass = EquipWeapon;
}

void UWeaponInventoryItem::SetStartedAmmoAmount(int32 NewStartedAmmoAmount)
{
	this->StartedAmmoAmount = NewStartedAmmoAmount;
}

int32 UWeaponInventoryItem::GetStartedAmmoAmount() const
{
	return StartedAmmoAmount;
}

bool UWeaponInventoryItem::Consume(AGCBaseCharacter* ConsumeTarget)
{
	return false;
}

TSubclassOf<AEquipableItem> UWeaponInventoryItem::GetEquipWeaponClass() const
{
	return EquipWeaponClass;
}
