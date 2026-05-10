// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OxygenManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOxygenDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOxygenRestored);

class UCharacterAttributeComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UOxygenManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOxygenManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Публичные методы для управления кислородом
	UFUNCTION(BlueprintCallable, Category = "Oxygen")
	void StartOxygenDrain();

	UFUNCTION(BlueprintCallable, Category = "Oxygen")
	void StopOxygenDrain();

	UFUNCTION(BlueprintCallable, Category = "Oxygen")
	void RestoreOxygen();

	UFUNCTION(BlueprintCallable, Category = "Oxygen")
	float GetCurrentOxygenPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Oxygen")
	bool IsDrainingOxygen() const { return bIsDrainingOxygen; }

	// Gameplay-события oxygen state. UI слушает CharacterAttributeComponent::OnOxygenChangedEvent.

	UPROPERTY(BlueprintAssignable, Category = "Oxygen")
	FOnOxygenDepleted OnOxygenDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Oxygen")
	FOnOxygenRestored OnOxygenRestored;

	// Геттеры для параметров
	float GetOxygenDrainRate() const { return OxygenDrainRate; }
	float GetOxygenRestoreRate() const { return OxygenRestoreRate; }
	float GetOxygenDepletionDamage() const { return OxygenDepletionDamage; }

	FLinearColor GetNormalOxygenColor() const { return NormalOxygenColor; }

protected:
	// Параметры кислорода

	UPROPERTY(EditAnywhere, Category = "Oxygen|Settings")
	float OxygenDrainRate = 20.0f; // Единиц кислорода в секунду под водой

	UPROPERTY(EditAnywhere, Category = "Oxygen|Settings")
	float OxygenRestoreRate = 30.0f; // Единиц кислорода в секунду на воздухе

	UPROPERTY(EditAnywhere, Category = "Oxygen|Settings")
	float OxygenRestoreDelay = 1.0f; // Секунд до начала восстановления

	UPROPERTY(EditAnywhere, Category = "Oxygen|UI")
	FLinearColor NormalOxygenColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
	// Параметры урона
	UPROPERTY(EditAnywhere, Category = "Oxygen|Damage")
	float OxygenDepletionDamage = 100.0f; // Урон при полном истощении кислорода

private:
	// Внутреннее состояние
	UPROPERTY()
	UCharacterAttributeComponent* CharacterAttributesComponent = nullptr;
	bool bIsDrainingOxygen;
	bool bIsRestoringOxygen;
	bool bIsOxygenDepleted;
	FTimerHandle RestoreDelayTimerHandle;

	// Вспомогательные методы
	void UpdateOxygen(float DeltaTime);
	void DrainOxygen(float DeltaTime);
	void RestoreOxygenInternal(float DeltaTime);
	void StartOxygenRestoration();
	void CheckOxygenDepleted();

	UFUNCTION()
	void OnRestoreDelayComplete();
};