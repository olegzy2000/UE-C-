// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() UCharacterEquipmentComponent can be used only with AGCBaseCharacter"))
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());
	CreateLoadout();
}
void UCharacterEquipmentComponent::CreateLoadout()
{
	if (!IsValid(SideArmClass)) {
		return;
	}
	CurrentEquippedWeapon = GetWorld()->SpawnActor<ARangeWeaponItem>(SideArmClass);
	CurrentEquippedWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketCharacterWeapon);
}
void UCharacterEquipmentComponent::Fire()
{
	if (IsValid(CurrentEquippedWeapon)) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("UCharacterEquipmentComponent::Fire()"));
		CurrentEquippedWeapon->Fire();
	}
}
EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedWeaponType() const {
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedWeapon)) {
		Result = CurrentEquippedWeapon->GetItemType();
	}
	return Result;
}

