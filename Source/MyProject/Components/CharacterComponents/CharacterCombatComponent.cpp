// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterCombatComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Throwable/ThrowableItem.h"

UCharacterCombatComponent::UCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

AGCBaseCharacter* UCharacterCombatComponent::GetBaseCharacterOwner() const
{
	return Cast<AGCBaseCharacter>(GetOwner());
}

UCharacterEquipmentComponent* UCharacterCombatComponent::GetEquipmentComponent() const
{
	const AGCBaseCharacter* BaseCharacter = GetBaseCharacterOwner();
	return IsValid(BaseCharacter) ? BaseCharacter->GetCharacterEquipmentComponent_Mutable() : nullptr;
}

void UCharacterCombatComponent::PreviousItem()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent)) {
		EquipmentComponent->EquipPreviousItem();
	}
}

void UCharacterCombatComponent::NextItem()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent)) {
		EquipmentComponent->EquipNextItem();
	}
}

void UCharacterCombatComponent::Reload()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent) && IsValid(EquipmentComponent->GetCurrentRangeWeaponItem())) {
		EquipmentComponent->ReloadCurrentWeapon();
	}
}

void UCharacterCombatComponent::ChangeFireMode()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent) && IsValid(EquipmentComponent->GetCurrentRangeWeaponItem())) {
		EquipmentComponent->GetCurrentRangeWeaponItem()->ChangeFireMode();
	}
}

void UCharacterCombatComponent::StartFire()
{
	if (!CanStartFire()) {
		return;
	}

	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!IsValid(EquipmentComponent)) {
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = EquipmentComponent->GetCurrentRangeWeaponItem();
	if (IsValid(CurrentRangeWeapon)) {
		CurrentRangeWeapon->StartFire();
		return;
	}

	AThrowableItem* CurrentThrowableItem = EquipmentComponent->GetCurrentThowableItem();
	if (IsValid(CurrentThrowableItem)) {
		EquipmentComponent->StartLaunching(CurrentThrowableItem->GetCharacterThrowAnimMontage());
		return;
	}
}

void UCharacterCombatComponent::StopFire()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!IsValid(EquipmentComponent)) {
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = EquipmentComponent->GetCurrentRangeWeaponItem();
	if (IsValid(CurrentRangeWeapon)) {
		CurrentRangeWeapon->StopFire();
	}
}

void UCharacterCombatComponent::StartAiming()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!IsValid(EquipmentComponent)) {
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = EquipmentComponent->GetCurrentRangeWeaponItem();
	if (!IsValid(CurrentRangeWeapon)) {
		return;
	}

	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAim();

	AGCBaseCharacter* BaseCharacter = GetBaseCharacterOwner();
	if (IsValid(BaseCharacter)) {
		BaseCharacter->OnStartAiming();
	}
}

void UCharacterCombatComponent::StopAiming()
{
	if (!bIsAiming) {
		return;
	}

	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent)) {
		ARangeWeaponItem* CurrentRangeWeapon = EquipmentComponent->GetCurrentRangeWeaponItem();
		if (IsValid(CurrentRangeWeapon)) {
			CurrentRangeWeapon->StopAim();
		}
	}

	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;

	AGCBaseCharacter* BaseCharacter = GetBaseCharacterOwner();
	if (IsValid(BaseCharacter)) {
		BaseCharacter->OnStopAiming();
	}
}

void UCharacterCombatComponent::PrimaryMeleeAttack()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!IsValid(EquipmentComponent)) {
		return;
	}

	AMeleeWeaponItem* CurrentMeleeWeapon = EquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon)) {
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}
}

void UCharacterCombatComponent::SecondaryMeleeAttack()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (!IsValid(EquipmentComponent)) {
		return;
	}

	AMeleeWeaponItem* CurrentMeleeWeapon = EquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon)) {
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}
}

void UCharacterCombatComponent::EquipPrimaryItem()
{
	UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	if (IsValid(EquipmentComponent)) {
		EquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrivaryItemSlot);
	}
}

bool UCharacterCombatComponent::CanStartFire() const
{
	const UCharacterEquipmentComponent* EquipmentComponent = GetEquipmentComponent();
	return IsValid(EquipmentComponent) && !EquipmentComponent->IsEquipping();
}

bool UCharacterCombatComponent::IsAiming() const
{
	return bIsAiming;
}

float UCharacterCombatComponent::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}
