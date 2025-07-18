// Fill out your copyright notice in the Description page of Project Settings.


#include "GCDataTableUtils.h"

FWeaponTableRow* GCDataTableUtils::FindWeaponData(FName WeaponID)
{
	static const FString contextString(TEXT("Find Weapon Data"));
	UDataTable* weaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));
	if (weaponDataTable == nullptr) {
		return nullptr;
	}
	return weaponDataTable->FindRow<FWeaponTableRow>(WeaponID, contextString);
}
FAmmoTableRow* GCDataTableUtils::FindAmmoData(FName AmmoID)
{
	static const FString contextString(TEXT("Find Ammo Data"));
	UDataTable* AmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Core/Data/DataTables/DT_AmmoList.DT_AmmoList"));
	if (AmmoDataTable == nullptr) {
		return nullptr;
	}
	return AmmoDataTable->FindRow<FAmmoTableRow>(AmmoID, contextString);
}


FItemTableRow* GCDataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString contextString(TEXT("Find item Data"));
	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));
	if (InventoryItemDataTable == nullptr) {
		return nullptr;
	}
	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, contextString);
}
