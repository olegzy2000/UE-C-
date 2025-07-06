#pragma once
#include "CoreMinimal.h"
class UInventoryAmmoItem;
class UInventoryItem;
class UWeaponInventoryItem;
class AGCBaseCharacter;
namespace GCSpawner
{
	TWeakObjectPtr<UInventoryAmmoItem> SpawnInventoryAmmoItem(AGCBaseCharacter* Character, FName DataTableID, int32 AmountAmmo);
	TWeakObjectPtr<UInventoryItem> SpawnInventoryItem(AGCBaseCharacter* Character, FName DataTableID);
	TWeakObjectPtr<UWeaponInventoryItem> SpawnInventoryWeaponItem(AGCBaseCharacter* Character, FName DataTableID);
};
