// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCodeTypes.h"
#include "Characters/GCBaseCharacter.h"
ARangeWeaponItem::ARangeWeaponItem() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzleSocket);
}

void ARangeWeaponItem::Fire()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::Fire() only character can be owner of range weapon"));
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller)) {
		return;
	}
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint,PlayerViewRotation);
	FVector ViewDirection= PlayerViewRotation.RotateVector(FVector::ForwardVector);
	WeaponBarell->Shot(PlayerViewPoint, ViewDirection, Controller);
}
