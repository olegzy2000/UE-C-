// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventary/InventoryItem.h"
#include "UInventoryAmmoItem.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UInventoryAmmoItem : public UInventoryItem
{
	GENERATED_BODY()
public:
	void SetAmmoType(EAmunitionType AmunitionType);
	void SetAmount(int32 Amount);
	EAmunitionType GetAmmoType() const;
	int32 GetAmount() const;
protected:
	UPROPERTY(EditAnywhere)
	EAmunitionType AmunitionType;
private:
	int32 Amount = 0;
};
