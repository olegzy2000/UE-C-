// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaRestored);

class UCharacterAttributeComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UStaminaManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Публичные методы для управления стаминой
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartStaminaDrain();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopStaminaDrain();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RestoreStamina();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetCurrentStaminaPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool CanSprint() const;


	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaDepleted OnStaminaDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaRestored OnStaminaRestored;
	FLinearColor GetNormalStaminaColor() const { return NormalStaminaColor; }
	float GetFatiguedWalkSpeed() const { return FatiguedWalkSpeed; }
	float GetNormalWalkSpeed() const { return NormalWalkSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool IsRecoveringFromFatigue() const { return bIsRecoveringFromFatigue; }


protected:
	UPROPERTY(EditAnywhere, Category = "Stamina|Settings")
	float StaminaDrainRate = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina|Settings")
	float StaminaRestoreRate = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina|Settings")
	float StaminaRestoreDelay = 1.0f;

	// Параметры движения при усталости
	UPROPERTY(EditAnywhere, Category = "Stamina|Movement")
	float NormalWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina|Movement")
	float FatiguedWalkSpeed = 300.0f;

	// UI параметры
	UPROPERTY(EditAnywhere, Category = "Stamina|UI")
	FLinearColor NormalStaminaColor = FLinearColor(0.066792f, 0.484279f, 1.0f, 1.0f);

private:
	bool bIsRecoveringFromFatigue;
	UPROPERTY()
	UCharacterAttributeComponent* CharacterAttributesComponent = nullptr;
	bool bIsDrainingStamina;
	bool bIsRestoringStamina;
	bool bCanSprint;
	FTimerHandle RestoreDelayTimerHandle;

	// Вспомогательные методы
	void UpdateStamina(float DeltaTime);
	void DrainStamina(float DeltaTime);
	void RestoreStaminaInternal(float DeltaTime);
	void StartStaminaRestoration();
	void CheckStaminaDepleted();

	UFUNCTION()
	void OnRestoreDelayComplete();
};