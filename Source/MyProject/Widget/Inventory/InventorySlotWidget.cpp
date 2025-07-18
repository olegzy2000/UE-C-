// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "../../Inventary/InventoryItem.h"
#include "Framework/Application/SlateApplication.h"
#include <Components/CharacterComponents/CharacterInventoryComponent.h>
#include <Runtime/UMG/Public/Components/Image.h>
#include <Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h>
#include <Inventary/Items/Ammo/UInventoryAmmoItem.h>

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr) {
		ImageItemIcon->SetBrushFromTexture(nullptr);
		SetAmount(0);
		return;
	}
	if (LinkedSlot->Item.IsValid()) {
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetDescription();
		ImageItemIcon->SetBrushFromTexture(Description.Icon);
		if (LinkedSlot->Item->IsA<UInventoryAmmoItem>()) {
			UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(LinkedSlot->Item);
			SetAmount(CurrentInventoryAmmoItem->GetAmount());
			if (CurrentInventoryAmmoItem->GetAmount() == 0)
				LinkedSlot->ClearSlot();
		}
	}
	else {
		ImageItemIcon->SetBrushFromTexture(nullptr);
		SetAmount(0);
	}
}
void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventarySlot) {
	LinkedSlot = &InventarySlot;
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
		LinkedSlot->UpdateSlotState();
		return FReply::Handled();
	}
	if (!LinkedSlot->Item.IsValid()) {
		LinkedSlot->UpdateSlotState();
		return FReply::Handled();
	}
	FKey MouseBtn = InMouseEvent.GetEffectingButton();
	if (MouseBtn == EKeys::RightMouseButton) {
		TWeakObjectPtr<UInventoryItem>LinkedSlotItem = LinkedSlot->Item;
		AGCBaseCharacter* ItemOwner = Cast<AGCBaseCharacter>(LinkedSlotItem->GetOuter());
		if (LinkedSlotItem->Consume(ItemOwner)) {
			LinkedSlot->ClearSlot();
		}
		return FReply::Handled();
	}
	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));
	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass());
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->Payload = LinkedSlot->Item.Get();
	OutOperation = DragOperation;

	LinkedSlot->ClearSlot();
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!LinkedSlot->Item.IsValid()) {
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
		LinkedSlot->UpdateSlotState();
		return true;
	}
	return false;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
	LinkedSlot->UpdateSlotState();
}
