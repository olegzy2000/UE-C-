// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <Inventary/InventoryItem.h>
#include "CoreMinimal.h"

namespace GCDataTableUtils
{
	FWeaponTableRow* FindWeaponData(FName WeaponID);
	FItemTableRow* FindInventoryItemData(const FName ItemID);
};
