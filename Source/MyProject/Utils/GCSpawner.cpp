#include "GCSpawner.h"
#include <Runtime/Core/Public/UObject/WeakObjectPtrTemplates.h>
#include "../Characters/GCBaseCharacter.h"
#include "../Inventary/InventoryItem.h"
#include "../Inventary/Items/Ammo/UInventoryAmmoItem.h"
#include "../Inventary/Items/Equipables/WeaponInventoryItem.h"
#include <Utils/GCDataTableUtils.h>

TWeakObjectPtr<UInventoryAmmoItem> GCSpawner::SpawnInventoryAmmoItem(AGCBaseCharacter* Character, FName DataTableID, int32 AmountAmmo)
{
	FAmmoTableRow* AmmoRow = GCDataTableUtils::FindAmmoData(DataTableID);
	TWeakObjectPtr<UInventoryAmmoItem> Ammo = nullptr;
	if (AmmoRow) {
		Ammo = NewObject<UInventoryAmmoItem>(Character, NAME_None, RF_Standalone);
		Ammo->Initialize(DataTableID, AmmoRow->InventoryItemDescription);
		Ammo->SetAmmoType(AmmoRow->AmunitionType);
		Ammo->SetAmount(AmountAmmo);
	}
	return Ammo;
}

TWeakObjectPtr<UInventoryItem> GCSpawner::SpawnInventoryItem(AGCBaseCharacter* Character, FName DataTableID)
{
	FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(DataTableID);
	if (ItemData == nullptr) {
		return nullptr;
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventroryItemClass));
	Item->Initialize(DataTableID, ItemData->InventoryItemDescription);
	return Item;
}

TWeakObjectPtr<UWeaponInventoryItem> GCSpawner::SpawnInventoryWeaponItem(AGCBaseCharacter* Character, FName DataTableID)
{
	FWeaponTableRow* WeaponRow = GCDataTableUtils::FindWeaponData(DataTableID);
	TWeakObjectPtr<UWeaponInventoryItem> Weapon = nullptr;
	if (WeaponRow) {
		Weapon = NewObject<UWeaponInventoryItem>(Character, NAME_None, RF_Standalone);
		Weapon->Initialize(DataTableID, WeaponRow->WeaponItemDescription);
		Weapon->SetEquipWeaponClass(WeaponRow->EquipableActor);
	}
	return Weapon;
}
