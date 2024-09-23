// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	ARangeWeaponItem();
	void Fire();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWeaponBarellComponent* WeaponBarell;
	
};
