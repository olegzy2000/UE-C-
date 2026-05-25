// Fill out your copyright notice in the Description page of Project Settings.


#include "Adrenaline.h"
#include "../../../Characters/GCBaseCharacter.h"
#include "../../../Components/CharacterComponents/CharacterAttributeComponent.h"
bool UAdrenaline::Consume(AGCBaseCharacter* ConsumeTarget) {
	UCharacterAttributeComponent* CharacterAttributeComponent = ConsumeTarget->GetCharacterAttributesComponent();
	CharacterAttributeComponent->RestoreFullStamina();
	this->ConditionalBeginDestroy();
	return true;
}
