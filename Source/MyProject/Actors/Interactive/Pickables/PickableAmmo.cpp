// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableAmmo.h"
#include <Utils/GCDataTableUtils.h>
#include <Inventary/Items/Ammo/UInventoryAmmoItem.h>
APickableAmmo::APickableAmmo() {
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(AmmoMesh);
}
void APickableAmmo::Interact(AGCBaseCharacter* Character)
{
	FAmmoTableRow* AmmoRow = GCDataTableUtils::FindAmmoData(DataTableID);
	if (AmmoRow) {
		TWeakObjectPtr<UInventoryAmmoItem> Ammo = NewObject<UInventoryAmmoItem>(Character, NAME_None, RF_Standalone);
		Ammo->Initialize(DataTableID, AmmoRow->InventoryItemDescription);
		Ammo->SetAmmoType(AmmoRow->AmunitionType);
		Ammo->SetAmount(AmountAmmo);
		Character->PickupItem(Ammo);
		Destroy();
	}
}

FName APickableAmmo::GetActionEventName() const
{
	return ActionInteract;
}

int32 APickableAmmo::GetAmountAmmo() const
{
	return AmountAmmo;
}

void APickableAmmo::SetAmountAmmo(int32 Ammo)
{
	this->AmountAmmo = Ammo;
}
