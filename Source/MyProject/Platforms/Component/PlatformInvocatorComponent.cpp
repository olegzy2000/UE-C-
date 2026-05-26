// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocatorComponent.h"

UPlatformInvocatorComponent::UPlatformInvocatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlatformInvocatorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(PlatformInvocator)) {
		PlatformInvocator->InvocatorActivated.AddUObject(this, &UPlatformInvocatorComponent::Invoke);
	}
	
}

void UPlatformInvocatorComponent::Invoke()
{
	if (InvocatorClient.IsBound()) {
		InvocatorClient.Broadcast();
	}
}
