// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedEvent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedEvent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOxygenChangedEvent, float);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterAttributeComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	UCharacterAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;

	// Percent-based attribute events. Value range is 0.0f - 1.0f.
	FOnHealthChangedEvent OnHealthChangedEvent;
	FOnStaminaChangedEvent OnStaminaChangedEvent;
	FOnOxygenChangedEvent OnOxygenChangedEvent;

	bool IsAlive() const;

	float GetMaxHealth() const;
	float GetHealth() const;
	float GetHealthPercent() const;

	float GetMaxStamina() const;
	float GetStamina() const;
	float GetStaminaPercent() const;

	float GetMaxOxygen() const;
	float GetOxygen() const;
	float GetOxygenPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes|Health")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Health")
	void AddHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Stamina")
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Stamina")
	void AddStamina(float StaminaToAdd);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Stamina")
	void RestoreFullStamina();

	UFUNCTION(BlueprintCallable, Category = "Attributes|Oxygen")
	void SetOxygen(float NewOxygen);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Oxygen")
	void AddOxygen(float OxygenToAdd);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Oxygen")
	void RestoreFullOxygen();

	virtual void OnLevelDeserialized_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health", meta = (ClampMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Stamina", meta = (ClampMin = 0.0f))
	float MaxStamina = 100.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen", meta = (ClampMin = 0.0f))
	float MaxOxygen = 100.0f;


private:
	UPROPERTY(SaveGame)
	float Health = 0.0f;

	UPROPERTY(SaveGame)
	float Stamina = 0.0f;

	UPROPERTY(SaveGame)
	float Oxygen = 0.0f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void BroadcastHealthChanged();
	void BroadcastStaminaChanged();
	void BroadcastOxygenChanged();
	void BroadcastAllAttributesChanged();

	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacterOwner;
};
