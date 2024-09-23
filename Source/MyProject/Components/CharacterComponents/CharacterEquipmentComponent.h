// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/GCBaseCharacter.h"
#include "CharacterEquipmentComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	void Fire();
	EEquipableItemType GetCurrentEquippedWeaponType() const;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TSubclassOf<ARangeWeaponItem>SideArmClass;
private:
	void CreateLoadout();
	ARangeWeaponItem* CurrentEquippedWeapon;
	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;

};
