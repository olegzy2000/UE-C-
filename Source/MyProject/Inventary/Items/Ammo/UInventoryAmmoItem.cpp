// Fill out your copyright notice in the Description page of Project Settings.


#include "UInventoryAmmoItem.h"

void UUInventoryAmmoItem::SetAmmoType(EAmunitionType NewAmunitionType) {
    AmunitionType = NewAmunitionType;
}

void UUInventoryAmmoItem::SetAmount(int32 NewAmount)
{
    this->Amount = NewAmount;
}

EAmunitionType UUInventoryAmmoItem::GetAmmoType()
{
    return AmunitionType;
}

int32 UUInventoryAmmoItem::GetAmount()
{
    return Amount;
}
