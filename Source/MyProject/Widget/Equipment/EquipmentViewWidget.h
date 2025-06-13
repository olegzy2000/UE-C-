// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentViewWidget.generated.h"

class UCharacterEquipmentComponent;
class UVerticalBox;
class UEquipmentSlotWidget;
class AEquipableItem;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UEquipmentViewWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent);
protected:
	void AddEquipmentSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex);
	void UpdateSlot(int32 SlotIndex);

	bool EquipEquipmentToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex);
	void RemoveEquipmentFromSlot(int32 SlotIndex);
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VBWeaponSlots;
	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
	TSubclassOf<UEquipmentSlotWidget> DefaultSlotViewClass;
	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;

};
