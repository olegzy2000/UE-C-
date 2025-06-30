// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryViewWidget.h"
#include "../../Inventary/InventoryItem.h"
#include "InventorySlotWidget.h"
#include "Components/GridPanel.h"
#include <Components/CharacterComponents/CharacterInventoryComponent.h>
void UInventoryViewWidget::AddItemSlotView(FInventorySlot& SlotToAdd)
{
	checkf(InventorySlotWidgetClass.Get() != nullptr, TEXT("UItemContainerWidget::AddItemSlotView widget"))
	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);

	if (SlotWidget != nullptr) {
		SlotWidget->InitializeItemSlot(SlotToAdd);
		const int32 CurrentSlotCount = GridPanelItemSlots->GetChildrenCount();
		const int32 CurrentSlotRow = CurrentSlotCount / ColumnsCount;
		const int32 CurrentSlotColumn = CurrentSlotCount % ColumnsCount;
		GridPanelItemSlots->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn);

		SlotWidget->UpdateView();
	}
	
}
void UInventoryViewWidget::InitializeViewWidget(TArray<FInventorySlot>& InventorySlots) {
	for (FInventorySlot& Item : InventorySlots) {
		AddItemSlotView(Item);
	}
}
