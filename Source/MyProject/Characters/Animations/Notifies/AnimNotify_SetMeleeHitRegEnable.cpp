// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animations/Notifies/AnimNotify_SetMeleeHitRegEnable.h"
#include <Characters/GCBaseCharacter.h>
#include <Components/CharacterComponents/CharacterEquipmentComponent.h>
#include <Actors/Equipment/Weapons/MeleeWeaponItem.h>

void UAnimNotify_SetMeleeHitRegEnable::Notify(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference
);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner)) {
		return;
	}
	AMeleeWeaponItem* MeleeWeapon=CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeapon)) {
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
