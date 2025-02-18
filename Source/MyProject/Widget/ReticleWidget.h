// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameCodeTypes.h>
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

/**
 * 
 */
class AEquipableItem;
UCLASS()
class MYPROJECT_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintNativeEvent)
		void OnAimingStateChange(bool bIsAming);
	UFUNCTION(BlueprintNativeEvent)
		void OnEquippedItemChanged(const AEquipableItem* EquippedItem);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
		EReticleType CurrentReticle;
private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;
	void SetupCurrentReticle();
};
