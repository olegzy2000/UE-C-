// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocatorComponent.h"

// Sets default values for this component's properties
UPlatformInvocatorComponent::UPlatformInvocatorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
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
