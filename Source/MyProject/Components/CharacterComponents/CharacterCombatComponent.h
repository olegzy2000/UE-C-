// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include "CharacterCombatComponent.generated.h"

class AGCBaseCharacter;
class UCharacterEquipmentComponent;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();

	virtual void BeginPlay() override;

	void PreviousItem();
	void NextItem();
	void Reload();
	void ChangeFireMode();

	void StartFire();
	void StopFire();
	void StartAiming();
	void StopAiming();
	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();
	void EquipPrimaryItem();

	bool CanStartFire() const;
	bool IsAiming() const;
	float GetAimingMovementSpeed() const;

private:
	AGCBaseCharacter* GetBaseCharacterOwner() const;
	UCharacterEquipmentComponent* GetEquipmentComponent() const;

	UPROPERTY()
	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;

	UPROPERTY()
	TWeakObjectPtr<UCharacterEquipmentComponent> CachedEquipmentComponent;

	bool bIsAiming = false;
	float CurrentAimingMovementSpeed = 0.0f;
};
