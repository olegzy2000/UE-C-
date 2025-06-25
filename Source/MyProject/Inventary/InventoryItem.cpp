// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"

void UInventoryItem::BeginDestroy()
{
    if (IsRooted())
    {
        RemoveFromRoot();
    }
    Super::BeginDestroy();
}

FName UInventoryItem::GetDataTableID() const
{
    return DataTableID;
}
void UInventoryItem::Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In) {
    DataTableID = DataTableID_In;
    Description.Icon = Description_In.Icon;
    Description.Name = Description_In.Name;
}
const FInventoryItemDescription& UInventoryItem::GetDescription() const
{
    return Description;
}

bool UInventoryItem::IsEquipable() const
{
    return bIsEquipable;
}

bool UInventoryItem::IsConsumable() const
{
    return bIsConsumable;
}
