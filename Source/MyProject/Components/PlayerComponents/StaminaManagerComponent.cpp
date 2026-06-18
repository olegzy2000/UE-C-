// Fill out your copyright notice in the Description page of Project Settings.

#include "StaminaManagerComponent.h"
#include "MyProject.h"

#include "Components/CharacterComponents/CharacterAttributeComponent.h"

UStaminaManagerComponent::UStaminaManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	bIsDrainingStamina = false;
	bIsRestoringStamina = false;
	bCanSprint = true;
	bIsRecoveringFromFatigue = false;
}

void UStaminaManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributesComponent = GetOwner() ? GetOwner()->FindComponentByClass<UCharacterAttributeComponent>() : nullptr;

	if (!CharacterAttributesComponent)
	{
		UE_LOG(LogCharacter, Warning, TEXT("UStaminaManagerComponent::BeginPlay - CharacterAttributeComponent not found on owner %s"), *GetNameSafe(GetOwner()));
		return;
	}

	CharacterAttributesComponent->RestoreFullStamina();
}

void UStaminaManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDrainingStamina || bIsRestoringStamina)
	{
		UpdateStamina(DeltaTime);
	}
}

void UStaminaManagerComponent::StartStaminaDrain()
{
	if (!CanSprint())
	{
		return;
	}

	if (bIsRestoringStamina)
	{
		bIsRestoringStamina = false;
	}

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RestoreDelayTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreDelayTimerHandle);
	}

	bIsDrainingStamina = true;
}

void UStaminaManagerComponent::StopStaminaDrain()
{
	bIsDrainingStamina = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RestoreDelayTimerHandle, this, &UStaminaManagerComponent::OnRestoreDelayComplete, StaminaRestoreDelay, false);
	}
}

void UStaminaManagerComponent::RestoreStamina()
{
	if (CharacterAttributesComponent)
	{
		CharacterAttributesComponent->RestoreFullStamina();
	}

	bIsRestoringStamina = false;
	bIsDrainingStamina = false;
	bCanSprint = true;
	bIsRecoveringFromFatigue = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreDelayTimerHandle);
	}

	OnStaminaRestored.Broadcast();
}

float UStaminaManagerComponent::GetCurrentStaminaPercent() const
{
	return CharacterAttributesComponent ? CharacterAttributesComponent->GetStaminaPercent() : 0.0f;
}

bool UStaminaManagerComponent::CanSprint() const
{
	return bCanSprint && GetCurrentStaminaPercent() > 0.0f;
}

void UStaminaManagerComponent::UpdateStamina(float DeltaTime)
{
	if (bIsDrainingStamina)
	{
		DrainStamina(DeltaTime);
	}
	else if (bIsRestoringStamina)
	{
		RestoreStaminaInternal(DeltaTime);
	}
}

void UStaminaManagerComponent::DrainStamina(float DeltaTime)
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	const float StaminaToDrain = StaminaDrainRate * DeltaTime;
	CharacterAttributesComponent->AddStamina(-StaminaToDrain);

	CheckStaminaDepleted();
}

void UStaminaManagerComponent::RestoreStaminaInternal(float DeltaTime)
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	const float StaminaToRestore = StaminaRestoreRate * DeltaTime;
	CharacterAttributesComponent->AddStamina(StaminaToRestore);


	if (CharacterAttributesComponent->GetStamina() >= CharacterAttributesComponent->GetMaxStamina())
	{
		bIsRestoringStamina = false;
		bCanSprint = true;
		bIsRecoveringFromFatigue = false;
		OnStaminaRestored.Broadcast();
	}
}

void UStaminaManagerComponent::StartStaminaRestoration()
{
	if (!CharacterAttributesComponent)
	{
		return;
	}

	if (!bIsDrainingStamina && CharacterAttributesComponent->GetStamina() < CharacterAttributesComponent->GetMaxStamina())
	{
		bIsRestoringStamina = true;
	}
}

void UStaminaManagerComponent::CheckStaminaDepleted()
{
	if (GetCurrentStaminaPercent() <= 0.0f && bCanSprint)
	{
		bCanSprint = false;
		bIsDrainingStamina = false;
		bIsRecoveringFromFatigue = true;
		OnStaminaDepleted.Broadcast();
	}
}


void UStaminaManagerComponent::OnRestoreDelayComplete()
{
	StartStaminaRestoration();
}
