// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickableAmmo.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT_API APickableAmmo : public APickableItem
{
	GENERATED_BODY()
public:
	APickableAmmo();
	virtual void Interact(AGCBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;
	int32 GetAmountAmmo() const;
	void SetAmountAmmo(int32 Ammo);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* AmmoMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 AmountAmmo;
};
