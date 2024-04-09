// Fill out your copyright notice in the Description page of Project Settings.


#include "PayerGameModeBaseSecondVersion.h"
void APayerGameModeBaseSecondVersion::BeginPlay()
{
	Super::BeginPlay();
	if (FatigueBar != nullptr) {
		CurrentWidget = CreateWidget<UFatigueBar>(GetWorld(), FatigueBar);
		//CurrentWidget = static_cast<UFatigueBar*>(CurrentWidget);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}
}

UFatigueBar* APayerGameModeBaseSecondVersion::GetCurrentWidget()
{
	return CurrentWidget;
}
