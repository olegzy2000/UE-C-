// Fill out your copyright notice in the Description page of Project Settings.


#include "UInventoryAmmoItem.h"

void UInventoryAmmoItem::SetAmmoType(EAmunitionType NewAmunitionType) {
    AmunitionType = NewAmunitionType;
}

void UInventoryAmmoItem::SetAmount(int32 NewAmount)
{
    this->Amount = NewAmount;
}

EAmunitionType UInventoryAmmoItem::GetAmmoType()
{
    return AmunitionType;
}

int32 UInventoryAmmoItem::GetAmount()
{
    return Amount;
}
