// Fill out your copyright notice in the Description page of Project Settings.


#include "GCCharacterAttributeSet.h"

void UGCCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute.AttributeName == FString("Stamina")) {
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxStamina.GetBaseValue());
	}
}
float UGCCharacterAttributeSet::GetStaminaPercent() const
{
	return Stamina.GetCurrentValue() / MaxStamina.GetCurrentValue();
}
void UGCCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute.AttributeName == FString("Stamina")) {
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxStamina.GetBaseValue());
	}
 }