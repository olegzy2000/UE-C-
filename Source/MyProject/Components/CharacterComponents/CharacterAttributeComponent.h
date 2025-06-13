// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedEvent,float);
DECLARE_MULTICAST_DELEGATE(FOnHealthAddEvent);
DECLARE_MULTICAST_DELEGATE(FOnRestoreStaminaEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnDeathEventSignature OnDeathEvent;
	FOnHealthChangedEvent OnHealthChangedEvent;
	FOnHealthAddEvent OnHealthAddEvent;
	FOnRestoreStaminaEvent OnRestoreStaminaEvent;
	bool IsAlive();
	float GetMaxHealth();
	float GetHealth();
	float GetMaxStamina();
	float GetSpeedDownStamina();
	float GetSpeedUpStamina();
	float GetSpeedUpStaminaAfterZeroValue();

	float GetMaxOxygen();
	float GetOxygenRestoreVelocity();
	float GetSwimOxygenConsumptionVelocity();
	UFUNCTION()
	void AddHealth(float HealthToAdd);
	void RestoreFullStamina();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.9f))
		float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	   float MaxStamina = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin=0.0f,ClampMax=100.0f))
	    float SpeedDownStamina = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
		float SpeedUpStamina = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
		float SpeedUpStaminaAfterZeroValue = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
		float MaxOxygen = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
		float OxygenRestoreVelocity = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
		float SwimOxygenConsumptionVelocity = 20.0f;
private:
	float Health = 0.0f;
	float Stamina = 0.0f;
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	TWeakObjectPtr<class AGCBaseCharacter>CachedBaseCharacterOwner;
};
