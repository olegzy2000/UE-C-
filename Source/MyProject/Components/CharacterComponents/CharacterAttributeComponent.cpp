// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAttributeComponent.h"
#include "MyProject.h"
#include "Engine/GameInstance.h"

#include "../../Characters/GCBaseCharacter.h"
#include "../../GameCodeTypes.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Public/DrawDebugHelpers.h>

UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	AGCBaseCharacter* OwnerCharacter = Cast<AGCBaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogCharacter, Warning, TEXT("UCharacterAttributeComponent::BeginPlay failed: owner is not AGCBaseCharacter | Owner=%s"), *GetNameSafe(GetOwner()));
		return;
	}

	CachedBaseCharacterOwner = OwnerCharacter;

	Health = MaxHealth;
	Stamina = MaxStamina;
	Oxygen = MaxOxygen;

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddUniqueDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);

	BroadcastAllAttributesChanged();
}

void UCharacterAttributeComponent::OnLevelDeserialized_Implementation()
{
	BroadcastAllAttributesChanged();
}

void UCharacterAttributeComponent::SetHealth(float NewHealth)
{
	const bool bWasAlive = IsAlive();
	const float OldHealth = Health;

	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

	if (!FMath::IsNearlyEqual(OldHealth, Health))
	{
		BroadcastHealthChanged();
	}

	if (bWasAlive && !IsAlive())
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

void UCharacterAttributeComponent::AddHealth(float HealthToAdd)
{
	SetHealth(Health + HealthToAdd);
}

void UCharacterAttributeComponent::SetStamina(float NewStamina)
{
	const float OldStamina = Stamina;
	Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);

	if (!FMath::IsNearlyEqual(OldStamina, Stamina))
	{
		BroadcastStaminaChanged();
	}
}

void UCharacterAttributeComponent::AddStamina(float StaminaToAdd)
{
	SetStamina(Stamina + StaminaToAdd);
}

void UCharacterAttributeComponent::RestoreFullStamina()
{
	SetStamina(MaxStamina);
}

void UCharacterAttributeComponent::SetOxygen(float NewOxygen)
{
	const float OldOxygen = Oxygen;
	Oxygen = FMath::Clamp(NewOxygen, 0.0f, MaxOxygen);

	if (!FMath::IsNearlyEqual(OldOxygen, Oxygen))
	{
		BroadcastOxygenChanged();
	}
}

void UCharacterAttributeComponent::AddOxygen(float OxygenToAdd)
{
	SetOxygen(Oxygen + OxygenToAdd);
}

void UCharacterAttributeComponent::RestoreFullOxygen()
{
	SetOxygen(MaxOxygen);
}

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	const FString DamageCauserName = IsValid(DamageCauser) ? DamageCauser->GetName() : FString(TEXT("Unknown"));
	const FString OwnerName = CachedBaseCharacterOwner.IsValid() ? CachedBaseCharacterOwner->GetName() : FString(TEXT("Unknown"));

	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s received %.2f amount of damage from %s"), *OwnerName, Damage, *DamageCauserName);

	SetHealth(Health - Damage);

	if (!IsAlive())
	{
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage character %s is killed by an actor %s"), *OwnerName, *DamageCauserName);
	}
}

void UCharacterAttributeComponent::BroadcastHealthChanged()
{
	OnHealthChangedEvent.Broadcast(GetHealthPercent());
}

void UCharacterAttributeComponent::BroadcastStaminaChanged()
{
	OnStaminaChangedEvent.Broadcast(GetStaminaPercent());
}

void UCharacterAttributeComponent::BroadcastOxygenChanged()
{
	OnOxygenChangedEvent.Broadcast(GetOxygenPercent());
}

void UCharacterAttributeComponent::BroadcastAllAttributesChanged()
{
	BroadcastHealthChanged();
	BroadcastStaminaChanged();
	BroadcastOxygenChanged();
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = IsValid(World) ? UGameplayStatics::GetGameInstance(World) : nullptr;
	UDebugSubsystem* DebugSubsystem = IsValid(GameInstance) ? GameInstance->GetSubsystem<UDebugSubsystem>() : nullptr;
	if (!IsValid(DebugSubsystem) || !DebugSubsystem->IsCategoryEnable(DebugCategoryCharacterAttributes))
	{
		return;
	}

	if (!CachedBaseCharacterOwner.IsValid() || !IsValid(CachedBaseCharacterOwner->GetCapsuleComponent()))
	{
		return;
	}

	const FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(
		GetWorld(),
		TextLocation,
		FString::Printf(TEXT("Health: %.2f\nStamina: %.2f\nOxygen: %.2f"), Health, Stamina, Oxygen),
		nullptr,
		FColor::Green,
		0.0f,
		true
	);
}
#endif

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

bool UCharacterAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

float UCharacterAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UCharacterAttributeComponent::GetHealth() const
{
	return Health;
}

float UCharacterAttributeComponent::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? Health / MaxHealth : 0.0f;
}

float UCharacterAttributeComponent::GetMaxStamina() const
{
	return MaxStamina;
}

float UCharacterAttributeComponent::GetStamina() const
{
	return Stamina;
}

float UCharacterAttributeComponent::GetStaminaPercent() const
{
	return MaxStamina > 0.0f ? Stamina / MaxStamina : 0.0f;
}

float UCharacterAttributeComponent::GetMaxOxygen() const
{
	return MaxOxygen;
}

float UCharacterAttributeComponent::GetOxygen() const
{
	return Oxygen;
}

float UCharacterAttributeComponent::GetOxygenPercent() const
{
	return MaxOxygen > 0.0f ? Oxygen / MaxOxygen : 0.0f;
}

