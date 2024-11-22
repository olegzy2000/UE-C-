// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCodeTypes.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
UCLASS(Abstract,NotBlueprintable)
class MYPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

  protected:
	  UPROPERTY(BlueprintAssignable)
		  FOnEquipmentStateChanged OnEquipmentStateChanged;
	  UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Equipable item")
	  EEquipableItemType EquipableItemType=EEquipableItemType::None;
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
		  FName UnEquppedSocketName = NAME_None;
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
		  FName EquppedSocketName = NAME_None;
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
		  int32 MaxAmmo = 30;
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
		  EAmunitionType AmmoType;
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
		  UAnimMontage* CharacterEquipAnimMontage;
  public:
	  EEquipableItemType GetItemType();
	  FName GetUnEquppedSocketName() const;
	  FName GetEquppedSocketName() const;
	  UAnimMontage* GetCharacterEquipAnimMontage() const;
	  virtual void Equip();
	  virtual void UnEquip();
	  EAmunitionType GetAmmoType() const;
	  int32 GetMaxAmmo() const;
	  int32 GetAmmo() const;
	  void SetAmmo(int32 Ammo);
	  void SetMaxAmmo(int32 Ammo);
private:
	int32 Ammo = 0;
};
