// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableWeapon.h"
#include <GameCodeTypes.h>
#include <Utils/GCDataTableUtils.h>

APickableWeapon::APickableWeapon() {
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}
void APickableWeapon::Interact(AGCBaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = GCDataTableUtils::FindWeaponData(DataTableID);
	if (WeaponRow) {
			Character->AddEquipmentItem(WeaponRow->EquipableActor);
			Destroy();
	}
}
FName APickableWeapon::GetActionEventName() const {
	return ActionInteract;
}