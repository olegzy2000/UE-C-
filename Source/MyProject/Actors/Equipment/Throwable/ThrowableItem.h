// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	void Throw();
	UAnimMontage* GetCharacterThrowAnimMontage() const;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
		TSubclassOf<class AGCProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
		UAnimMontage* CharacterThrowAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f,ClampMin=-90.0f,ClampMax=90.0f))
		float ThrowAngle = 0.0f;
};
