// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableWeapon.h"
#include <GameCodeTypes.h>
#include <Utils/GCDataTableUtils.h>
#include <Inventary/Items/Equipables/WeaponInventoryItem.h>
#include <Utils/GCSpawner.h>

APickableWeapon::APickableWeapon() {
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}
void APickableWeapon::Interact(AGCBaseCharacter* Character)
{
	/*FWeaponTableRow* WeaponRow = GCDataTableUtils::FindWeaponData(DataTableID);
	if (WeaponRow) {
			TWeakObjectPtr<UWeaponInventoryItem> Weapon = NewObject<UWeaponInventoryItem>(Character, NAME_None, RF_Standalone);
			Weapon->Initialize(DataTableID, WeaponRow->WeaponItemDescription);
			Weapon->SetEquipWeaponClass(WeaponRow->EquipableActor);
			Character->PickupItem(Weapon);
			Destroy();
	}*/
	TWeakObjectPtr<UWeaponInventoryItem> Weapon = GCSpawner::SpawnInventoryWeaponItem(Character, DataTableID);
	Weapon->SetStartedAmmoAmount(StartedAmmoAmount);
	Character->PickupItem(Weapon);
	Destroy();
}
FName APickableWeapon::GetActionEventName() const {
	return ActionInteract;
}