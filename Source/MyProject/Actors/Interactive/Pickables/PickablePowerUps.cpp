// Fill out your copyright notice in the Description page of Project Settings.


#include "PickablePowerUps.h"
#include <GameCodeTypes.h>
#include "../../../Utils/GCDataTableUtils.h"
#include "../../../Characters/GCBaseCharacter.h"
#include <Components/CharacterComponents/CharacterInventoryComponent.h>
#include <Utils/GCSpawner.h>

APickablePowerUps::APickablePowerUps() {
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	SetRootComponent(PowerupMesh);
}
void APickablePowerUps::Interact(AGCBaseCharacter* Character)
{
	/*FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(GetDataTableID());
	if (ItemData == nullptr) {
		return;
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventroryItemClass));
	Item->Initialize(DataTableID, ItemData->InventoryItemDescription);
	*/
	TWeakObjectPtr<UInventoryItem> Item = GCSpawner::SpawnInventoryItem(Character, DataTableID);
	const bool bPickedUp = Character->PickupItem(Item);
	if (bPickedUp) {
		Destroy();
	}
}

FName APickablePowerUps::GetActionEventName() const
{
	return ActionInteract;
}
