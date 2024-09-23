// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCodeTypes.h"
ARangeWeaponItem::ARangeWeaponItem() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzleSocket);
}

void ARangeWeaponItem::Fire()
{
	WeaponBarell->Shot();
}
