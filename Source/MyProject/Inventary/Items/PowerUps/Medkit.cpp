// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventary/Items/PowerUps/Medkit.h"
#include "../../../Characters/GCBaseCharacter.h"
#include "../../../Components/CharacterComponents/CharacterAttributeComponent.h"
bool UMedkit::Consume(AGCBaseCharacter* ConsumeTarget) {
	UCharacterAttributeComponent* CharacterAttributeComponent= ConsumeTarget->GetCharacterAttributesComponent();
	CharacterAttributeComponent->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}