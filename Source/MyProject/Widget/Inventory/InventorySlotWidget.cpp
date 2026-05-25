// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "../../Inventory/InventoryItem.h"
#include "Framework/Application/SlateApplication.h"
#include <Components/CharacterComponents/CharacterInventoryComponent.h>
#include <Runtime/UMG/Public/Components/Image.h>
#include <Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h>
#include <Inventory/Items/Ammo/UInventoryAmmoItem.h>
#include <Characters/GCBaseCharacter.h>
#include <Utils/GCDataTableUtils.h>

namespace
{
	bool ResolveSlotDescription(const FInventorySlot& Slot, FInventoryItemDescription& OutDescription)
	{
		switch (Slot.GetItemType())
		{
		case EInventorySlotSaveType::AmmoItem:
		{
			const FAmmoTableRow* AmmoData = GCDataTableUtils::FindAmmoData(Slot.GetItemId());
			if (AmmoData != nullptr) {
				OutDescription = AmmoData->InventoryItemDescription;
				return true;
			}
			break;
		}
		case EInventorySlotSaveType::WeaponItem:
		{
			const FWeaponTableRow* WeaponData = GCDataTableUtils::FindWeaponData(Slot.GetItemId());
			if (WeaponData != nullptr) {
				OutDescription = WeaponData->WeaponItemDescription;
				return true;
			}
			break;
		}
		case EInventorySlotSaveType::GenericItem:
		{
			const FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(Slot.GetItemId());
			if (ItemData != nullptr) {
				OutDescription = ItemData->InventoryItemDescription;
				return true;
			}
			break;
		}
		default:
			break;
		}

		return false;
	}
}

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr || LinkedSlot->IsEmpty()) {
		ImageItemIcon->SetBrushFromTexture(nullptr);
		SetAmount(0);
		return;
	}

	FInventoryItemDescription Description;
	if (ResolveSlotDescription(*LinkedSlot, Description)) {
		ImageItemIcon->SetBrushFromTexture(Description.Icon);
	}
	else {
		ImageItemIcon->SetBrushFromTexture(nullptr);
	}

	if (LinkedSlot->GetItemType() == EInventorySlotSaveType::AmmoItem) {
		SetAmount(LinkedSlot->GetAmmoAmount());
		if (LinkedSlot->GetAmmoAmount() == 0) {
			LinkedSlot->ClearSlot();
		}
	}
	else {
		SetAmount(LinkedSlot->GetCount());
	}
}

void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventorySlot) {
	LinkedSlot = &InventorySlot;
	FInventorySlot::FInventorySlotUpdate OnInventorySlotUpdate;
	OnInventorySlotUpdate.BindUObject(this, &UInventorySlotWidget::UpdateView);
	LinkedSlot->BindOnInventorySlotUpdate(OnInventorySlotUpdate);
}

void UInventorySlotWidget::SetItemIcon(UTexture2D* Icon)
{
	ImageItemIcon->SetBrushFromTexture(Icon);
}

void UInventorySlotWidget::SetAmount(int32 NewAmount)
{
	if (NewAmount != 0) {
		FString Result = FString::Printf(TEXT("%i"), NewAmount);
		this->AmountTextBlock->SetText(FText::FromString(Result));
	}
	else {
		this->AmountTextBlock->SetText(FText::FromString(""));
	}

}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LinkedSlot == nullptr) {
		return FReply::Handled();
	}
	if (LinkedSlot->IsEmpty()) {
		LinkedSlot->UpdateSlotState();
		return FReply::Handled();
	}
	FKey MouseBtn = InMouseEvent.GetEffectingButton();
	if (MouseBtn == EKeys::RightMouseButton) {
		AGCBaseCharacter* ItemOwner = Cast<AGCBaseCharacter>(GetOwningPlayerPawn());
		TWeakObjectPtr<UInventoryItem> RuntimeItem = LinkedSlot->CreateRuntimeItem(ItemOwner);
		if (RuntimeItem.IsValid() && RuntimeItem->Consume(ItemOwner)) {
			LinkedSlot->ClearSlot();
		}
		return FReply::Handled();
	}
	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (LinkedSlot == nullptr || LinkedSlot->IsEmpty()) {
		return;
	}

	AGCBaseCharacter* ItemOwner = Cast<AGCBaseCharacter>(GetOwningPlayerPawn());
	TWeakObjectPtr<UInventoryItem> RuntimeItem = LinkedSlot->CreateRuntimeItem(ItemOwner);
	if (!RuntimeItem.IsValid()) {
		return;
	}

	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));
	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass());
	FInventoryItemDescription Description;
	if (ResolveSlotDescription(*LinkedSlot, Description)) {
		DragWidget->ImageItemIcon->SetBrushFromTexture(Description.Icon);
	}

	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Payload = RuntimeItem.Get();
	OutOperation = DragOperation;

	LinkedSlot->ClearSlot();
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (LinkedSlot != nullptr && LinkedSlot->IsEmpty()) {
		LinkedSlot->SetSlotItem(TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)), 1);
		return true;
	}
	return false;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (LinkedSlot != nullptr) {
		LinkedSlot->SetSlotItem(TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload)), 1);
	}
}
