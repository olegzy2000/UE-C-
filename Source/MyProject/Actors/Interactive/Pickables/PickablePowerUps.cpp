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
