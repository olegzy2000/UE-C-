// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventary/InventoryItem.h"
#include "Medkit.generated.h"
class AGCBaseCharacter;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()
public:
	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
		float Health = 25.0f;
};
