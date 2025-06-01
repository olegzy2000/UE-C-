// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"
#include "../../Characters/GCBaseCharacter.h"
#include "../../GameCodeTypes.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Public/DrawDebugHelpers.h>

UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


float UCharacterAttributeComponent::GetSpeedDownStamina()
{
	return SpeedDownStamina;
}

float UCharacterAttributeComponent::GetSpeedUpStamina()
{
	return SpeedUpStamina;
}

float UCharacterAttributeComponent::GetSpeedUpStaminaAfterZeroValue()
{
	return SpeedUpStaminaAfterZeroValue;
}

float UCharacterAttributeComponent::GetMaxOxygen()
{
	return MaxOxygen;
}

float UCharacterAttributeComponent::GetOxygenRestoreVelocity()
{
	return OxygenRestoreVelocity;
}

float UCharacterAttributeComponent::GetSwimOxygenConsumptionVelocity()
{
	return SwimOxygenConsumptionVelocity;
}

// Called when the game starts
void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay UCharacterAttributeComponent can be used only with AGCBaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	Health = MaxHealth;
	CachedBaseCharacterOwner->OnTakeAnyDamage.AddUniqueDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);

}
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (DebugSubsystem->IsCategoryEnable(DebugCategoryCharacterAttributes)) {
		return;
	}
	FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f)*FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
}
#endif
void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
		return;
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s recevied %.2f amount of damage form %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health =FMath::Clamp(Health-Damage,0.0f,MaxHealth);
	if (!IsAlive()) {
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage character %s is killed by an actor %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		if(OnDeathEvent.IsBound()){
			OnDeathEvent.Broadcast();
		}
	}
	if (OnHealthChangedEvent.IsBound()) {
		OnHealthChangedEvent.Broadcast(Health/MaxHealth);
	}
}


// Called every frame
void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

bool UCharacterAttributeComponent::IsAlive()
{
	return Health > 0.0f;
}

float UCharacterAttributeComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UCharacterAttributeComponent::GetHealth()
{
	return Health;
}

float UCharacterAttributeComponent::GetMaxStamina()
{
	return MaxStamina;
}

