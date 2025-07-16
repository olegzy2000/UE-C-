// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableAmmo.h"
#include <Utils/GCDataTableUtils.h>
#include <Inventary/Items/Ammo/UInventoryAmmoItem.h>
#include <Utils/GCSpawner.h>
APickableAmmo::APickableAmmo() {
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(AmmoMesh);
}
void APickableAmmo::Interact(AGCBaseCharacter* Character)
{
	TWeakObjectPtr<UInventoryAmmoItem> AmmoInventoryItem = GCSpawner::SpawnInventoryAmmoItem(Character,DataTableID,AmountAmmo);
	if (AmmoInventoryItem.IsValid()) {
		Character->PickupItem(AmmoInventoryItem);
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
