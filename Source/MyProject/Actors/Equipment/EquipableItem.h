// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCodeTypes.h"
#include <Characters/GCBaseCharacter.h>
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
	  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
		  EReticleType ReticleType=EReticleType::None;
	  EAmunitionType DefaultAmmoType;

	  AGCBaseCharacter* GetCharacterOwner();
  public:
	  EEquipableItemType GetItemType();
	  FName GetUnEquppedSocketName() const;
	  FName GetEquppedSocketName() const;
	  UAnimMontage* GetCharacterEquipAnimMontage() const;
	  virtual void Equip();
	  virtual void UnEquip();
	  virtual EReticleType GetReticleType() const;
	  virtual void SetOwner(AActor* NewOwner) override;
	  EAmunitionType GetAmmoType() const;
	  int32 GetMaxAmmo() const;
	  int32 GetCurrentAmmo() const;
	  void SetAmmo(int32 Ammo);
	  void SetMaxAmmo(int32 Ammo);
private:
	TWeakObjectPtr<AGCBaseCharacter> CharacterOwner;
	int32 CurrentAmmo = 0;
};
