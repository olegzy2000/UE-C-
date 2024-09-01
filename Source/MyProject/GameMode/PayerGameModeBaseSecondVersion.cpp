// Fill out your copyright notice in the Description page of Project Settings.


#include "PayerGameModeBaseSecondVersion.h"
void APayerGameModeBaseSecondVersion::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(StaminaBar)) {
		CurrentStaminaWidget = CreateWidget<UProgressBarWidget>(GetWorld(), StaminaBar);
		CurrentStaminaWidget->AddToViewport();
	}
	if (IsValid(HealthBar)) {
		CurrentHealthWidget = CreateWidget<UProgressBarWidget>(GetWorld(), HealthBar);
		CurrentHealthWidget->AddToViewport();
	}
	if (IsValid(OxygenBar)) {
		CurrentOxygenWidget = CreateWidget<UProgressBarWidget>(GetWorld(), OxygenBar);
		CurrentOxygenWidget->AddToViewport();
	}

}

UProgressBarWidget* APayerGameModeBaseSecondVersion::GetCurrentStaminaWidget()
{
	return CurrentStaminaWidget;
}

UProgressBarWidget* APayerGameModeBaseSecondVersion::GetCurrentHealthWidget()
{
	return CurrentHealthWidget;
}

UProgressBarWidget* APayerGameModeBaseSecondVersion::GetCurrentOxygenWidget()
{
	return CurrentOxygenWidget;
}
