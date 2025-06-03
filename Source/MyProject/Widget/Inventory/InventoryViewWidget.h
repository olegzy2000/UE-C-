// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryViewWidget.generated.h"
class UGridPanel;
class UInventorySlotWidget;
struct FInventorySlot;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UInventoryViewWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeViewWidget(TArray<FInventorySlot>& InventorySlots);
protected:
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelItemSlots;

	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Setting")
		TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "ItemContainer View Settings")
		int32 ColumnsCount = 4;
	void AddItemSlotView(FInventorySlot& SlotToAdd);

};
