// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animations/Notifies/AnimNotify_SetMeleeHitRegEnable.h"
#include <Characters/GCBaseCharacter.h>
#include <Components/CharacterComponents/CharacterEquipmentComponent.h>
#include <Actors/Equipment/Weapons/MeleeWeaponItem.h>

void UAnimNotify_SetMeleeHitRegEnable::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* AnimSequence)
{
	Super::Notify(Mesh, AnimSequence);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(Mesh->GetOwner());
	if (!IsValid(CharacterOwner)) {
		return;
	}
	AMeleeWeaponItem* MeleeWeapon=CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeapon)) {
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
