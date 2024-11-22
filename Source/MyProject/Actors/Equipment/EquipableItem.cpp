// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/EquipableItem.h"


EEquipableItemType AEquipableItem::GetItemType()
{
	return EquipableItemType;
}

FName AEquipableItem::GetUnEquppedSocketName() const
{
	return UnEquppedSocketName;
}

FName AEquipableItem::GetEquppedSocketName() const
{
	return EquppedSocketName;
}
UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}
void AEquipableItem::Equip() {
	if (OnEquipmentStateChanged.IsBound()) {
		OnEquipmentStateChanged.Broadcast(true);
	}
}
void AEquipableItem::UnEquip() {
	if (OnEquipmentStateChanged.IsBound()) {
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EAmunitionType AEquipableItem::GetAmmoType() const
{
	return AmmoType;
}

int32 AEquipableItem::GetAmmo() const
{
	return Ammo;
}

void AEquipableItem::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;
}
void AEquipableItem::SetMaxAmmo(int32 NewAmmo)
{
	MaxAmmo = NewAmmo;
}
int32 AEquipableItem::GetMaxAmmo() const
{
	return MaxAmmo;
}
