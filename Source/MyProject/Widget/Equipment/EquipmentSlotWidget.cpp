// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSlotWidget.h"
#include "../../Inventary/InventoryItem.h"
#include "../../Inventary/Items/Equipables/WeaponInventoryItem.h"
#include "../../Actors/Equipment/EquipableItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include <Utils/GCDataTableUtils.h>
#include "../Inventory/InventorySlotWidget.h"
#include <Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h>

void UEquipmentSlotWidget::InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 index)
{
	SlotIndexInComponent = index;
	if (!Equipment.IsValid()) {
		return;
	}
	LinkedEquipableItem = Equipment;
	

	FWeaponTableRow* EquipmentData = GCDataTableUtils::FindWeaponData(Equipment->GetDataTableID());
	if (EquipmentData != nullptr) {
		AdapterLinkedInventoryItem = NewObject<UWeaponInventoryItem>(Equipment->GetOwner() ,NAME_None, RF_Standalone);
		AdapterLinkedInventoryItem->Initialize(Equipment->GetDataTableID(), EquipmentData->WeaponItemDescription);
		AdapterLinkedInventoryItem->SetEquipWeaponClass(EquipmentData->EquipableActor);
	}
}

void UEquipmentSlotWidget::UpdateView()
{
	if (LinkedEquipableItem.IsValid()) {
		ImageWeaponIcon->SetBrushFromTexture(AdapterLinkedInventoryItem->GetDescription().Icon);
		TBWeaponName->SetText(AdapterLinkedInventoryItem->GetDescription().Name);
	}
	else {
		ImageWeaponIcon->SetBrushFromTexture(nullptr);
		TBWeaponName->SetText(FText::FromName(FName(NAME_None)));
	}
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!LinkedEquipableItem.IsValid()){
		return FReply::Handled();
    }
	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	checkf(DragAndDropWidgetClass.Get() != nullptr, TEXT("UEquipmentSlotWidget::NativeOnDragDetected drag and drop widget is"));
	if (!AdapterLinkedInventoryItem.IsValid()) {
		return;
	}
	UDragDropOperation* DragOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), DragAndDropWidgetClass);
	DragWidget->SetItemIcon(AdapterLinkedInventoryItem->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->Pivot = EDragPivot::CenterCenter;
	AdapterLinkedInventoryItem.Get()->SetStartedAmmoAmount(LinkedEquipableItem->GetCurrentAmmo());
	DragOperation->Payload = AdapterLinkedInventoryItem.Get();
	OutOperation = DragOperation;

	LinkedEquipableItem.Reset();
	OnEquipmentRemoveFromSlot.ExecuteIfBound(SlotIndexInComponent);
	UpdateView();
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UWeaponInventoryItem* OperationObject = Cast<UWeaponInventoryItem>(InOperation->Payload);
	if (IsValid(OperationObject)) {
		return OnEquipmentDropInSlot.Execute(OperationObject->GetEquipWeaponClass(), SlotIndexInComponent);
	}
	return false;
}

void UEquipmentSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	AdapterLinkedInventoryItem = Cast<UWeaponInventoryItem>(InOperation->Payload);
	OnEquipmentDropInSlot.Execute(AdapterLinkedInventoryItem->GetEquipWeaponClass(), SlotIndexInComponent);
}
