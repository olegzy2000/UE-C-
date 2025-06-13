// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/EquipableItem.h"


EEquipableItemType AEquipableItem::GetItemType()
{
	return EquipableItemType;
}

FName AEquipableItem::GetUnEquppedSocketName() const
{
	return UnEquppedSocketName;
}

FName AEquipableItem::GetEquppedSocketName() const
{
	return EquppedSocketName;
}
UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}
void AEquipableItem::Equip() {
	if (OnEquipmentStateChanged.IsBound()) {
		OnEquipmentStateChanged.Broadcast(true);
	}
}
void AEquipableItem::UnEquip() {
	if (OnEquipmentStateChanged.IsBound()) {
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

EAmunitionType AEquipableItem::GetAmmoType() const
{
	return AmmoType;
}

int32 AEquipableItem::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

void AEquipableItem::SetAmmo(int32 NewAmmo)
{
	CurrentAmmo = NewAmmo;
}
void AEquipableItem::SetMaxAmmo(int32 NewAmmo)
{
	MaxAmmo = NewAmmo;
}
FName AEquipableItem::GetDataTableID() const
{
	return DataTableID;
}
bool AEquipableItem::IsSlotCompatable(EEquipmentSlots Slot)
{
	return CompatableEquipmentSlots->Contains(Slot);
}
void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner)) {
		checkf(NewOwner->IsA<AGCBaseCharacter>(), TEXT("AEquipableItem::SetOwner only character can be owner of equipable item"));
		AGCBaseCharacter* CurrentCharacterOwner = StaticCast<AGCBaseCharacter*>(NewOwner);
		CharacterOwner = CurrentCharacterOwner;
	}
	else {
		CharacterOwner = nullptr;
	}
}
int32 AEquipableItem::GetMaxAmmo() const
{
	return MaxAmmo;
}
AGCBaseCharacter* AEquipableItem::GetCharacterOwner() 
{
	return CharacterOwner.IsValid() ? CharacterOwner.Get():nullptr;
}
