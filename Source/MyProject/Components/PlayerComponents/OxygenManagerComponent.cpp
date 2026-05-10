// Fill out your copyright notice in the Description page of Project Settings.

#include "OxygenManagerComponent.h"

#include "Components/CharacterComponents/CharacterAttributeComponent.h"

UOxygenManagerComponent::UOxygenManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	bIsDrainingOxygen = false;
	bIsRestoringOxygen = false;
	bIsOxygenDepleted = false;
}

void UOxygenManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributesComponent = GetOwner() ? GetOwner()->FindComponentByClass<UCharacterAttributeComponent>() : nullptr;

	if (!CharacterAttributesComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UOxygenManagerComponent::BeginPlay - CharacterAttributeComponent not found on owner %s"), *GetNameSafe(GetOwner()));
		return;
	}

	CharacterAttributesComponent->RestoreFullOxygen();
}

void UOxygenManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDrainingOxygen || bIsRestoringOxygen)
	{
		UpdateOxygen(DeltaTime);
	}
}

void UOxygenManagerComponent::StartOxygenDrain()
{
	if (bIsOxygenDepleted)
	{
		return;
	}

	if (bIsRestoringOxygen)
	{
		bIsRestoringOxygen = false;
	}

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RestoreDelayTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreDelayTimerHandle);
	}

	bIsDrainingOxygen = true;
	UE_LOG(LogTemp, Verbose, TEXT("StartOxygenDrain - Started draining oxygen"));
}

void UOxygenManagerComponent::StopOxygenDrain()
{
	bIsDrainingOxygen = false;

	if (!CharacterAttributesComponent || bIsOxygenDepleted || CharacterAttributesComponent->GetOxygen() >= CharacterAttributesComponent->GetMaxOxygen())
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RestoreDelayTimerHandle, this, &UOxygenManagerComponent::OnRestoreDelayComplete, OxygenRestoreDelay, false);
	}
}

void UOxygenManagerComponent::RestoreOxygen()
{
	if (CharacterAttributesComponent)
	{
		CharacterAttributesComponent->RestoreFullOxygen();
	}

	bIsRestoringOxygen = false;
	bIsDrainingOxygen = false;
	bIsOxygenDepleted = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreDelayTimerHandle);
	}

	OnOxygenRestored.Broadcast();
	UE_LOG(LogTemp, Verbose, TEXT("RestoreOxygen - Fully restored"));
}

float UOxygenManagerComponent::GetCurrentOxygenPercent() const
{
	return CharacterAttributesComponent ? CharacterAttributesComponent->GetOxygenPercent() : 0.0f;
}


void UOxygenManagerComponent::UpdateOxygen(float DeltaTime)
{
	if (bIsDrainingOxygen)
	{
		DrainOxygen(DeltaTime);
	}
	else if (bIsRestoringOxygen)
	{
		RestoreOxygenInternal(DeltaTime);
	}
}

void UOxygenManagerComponent::DrainOxygen(float DeltaTime)
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	const float OxygenToDrain = OxygenDrainRate * DeltaTime;
	CharacterAttributesComponent->AddOxygen(-OxygenToDrain);

	CheckOxygenDepleted();

	UE_LOG(LogTemp, Verbose, TEXT("DrainOxygen - Current: %f"), CharacterAttributesComponent->GetOxygen());
}

void UOxygenManagerComponent::RestoreOxygenInternal(float DeltaTime)
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	const float OxygenToRestore = OxygenRestoreRate * DeltaTime;
	CharacterAttributesComponent->AddOxygen(OxygenToRestore);


	if (CharacterAttributesComponent->GetOxygen() >= CharacterAttributesComponent->GetMaxOxygen())
	{
		bIsRestoringOxygen = false;
		bIsOxygenDepleted = false;
		OnOxygenRestored.Broadcast();
		UE_LOG(LogTemp, Verbose, TEXT("RestoreOxygenInternal - Fully restored"));
	}
}

void UOxygenManagerComponent::StartOxygenRestoration()
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	if (!bIsDrainingOxygen && CharacterAttributesComponent->GetOxygen() < CharacterAttributesComponent->GetMaxOxygen() && !bIsOxygenDepleted)
	{
		bIsRestoringOxygen = true;
		UE_LOG(LogTemp, Verbose, TEXT("StartOxygenRestoration - Restoration started, Current: %f"), CharacterAttributesComponent->GetOxygen());
	}
}

void UOxygenManagerComponent::CheckOxygenDepleted()
{
	if (GetCurrentOxygenPercent() <= 0.0f && !bIsOxygenDepleted)
	{
		bIsOxygenDepleted = true;
		bIsDrainingOxygen = false;
		bIsRestoringOxygen = false;
		OnOxygenDepleted.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("CheckOxygenDepleted - Oxygen depleted!"));
	}
}


void UOxygenManagerComponent::OnRestoreDelayComplete()
{
	StartOxygenRestoration();
}
