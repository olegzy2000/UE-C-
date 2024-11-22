// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo)
{
	this->Ammo = NewAmmo;
	this->TotalAmmo = NewTotalAmmo;
}
