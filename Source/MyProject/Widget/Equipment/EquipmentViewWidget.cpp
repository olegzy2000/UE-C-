// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentViewWidget.h"
#include <Widget/Equipment/EquipmentSlotWidget.h>
#include <Runtime/UMG/Public/Components/VerticalBox.h>
#include <Components/CharacterComponents/CharacterEquipmentComponent.h>

void UEquipmentViewWidget::AddEquipmentSlotView(AEquipableItem* LinkToWeapon, int32 SlotIndex)
{
	checkf(IsValid(DefaultSlotViewClass.Get()), TEXT("UEquipmentViewWidget::AddEquipmentSlotView equipment class is not valid"))
	UEquipmentSlotWidget* SlotWidget = CreateWidget<UEquipmentSlotWidget>(this, DefaultSlotViewClass);

	if (IsValid(SlotWidget)) {
		SlotWidget->InitializeEquipmentSlot(LinkToWeapon, SlotIndex);
		VBWeaponSlots->AddChildToVerticalBox(SlotWidget);
		SlotWidget->UpdateView();
		SlotWidget->OnEquipmentDropInSlot.BindUObject(this, &UEquipmentViewWidget::EquipEquipmentToSlot);
		SlotWidget->OnEquipmentRemoveFromSlot.BindUObject(this, &UEquipmentViewWidget::RemoveEquipmentFromSlot);
	}
}

void UEquipmentViewWidget::UpdateSlot(int32 SlotIndex)
{
	UEquipmentSlotWidget* WidgetToUpdate = Cast<UEquipmentSlotWidget>(VBWeaponSlots->GetChildAt(SlotIndex-1));
	if (IsValid(WidgetToUpdate)) {
		WidgetToUpdate->InitializeEquipmentSlot(LinkedEquipmentComponent->GetItems()[SlotIndex], SlotIndex);
		WidgetToUpdate->UpdateView();
	}
}

bool UEquipmentViewWidget::EquipEquipmentToSlot(const TSubclassOf<AEquipableItem>& WeaponClass, int32 SenderIndex, int32 StartedAmmo)
{
	const bool Result = LinkedEquipmentComponent->AddEquipmentItemToSlot(WeaponClass, SenderIndex, StartedAmmo);
	if (Result) {
		UpdateSlot(SenderIndex);
	}
	return Result;
}

void UEquipmentViewWidget::RemoveEquipmentFromSlot(int32 SlotIndex)
{
	LinkedEquipmentComponent->RemoveItemFromSlot(SlotIndex);
}

void UEquipmentViewWidget::InitializeEquipmentWidget(UCharacterEquipmentComponent* EquipmentComponent) {
	LinkedEquipmentComponent = EquipmentComponent;
	const TArray<AEquipableItem*>& Items = LinkedEquipmentComponent->GetItems();
	for (int32 Index = 1; Index < Items.Num(); ++Index) {
		AddEquipmentSlotView(Items[Index], Index);
	}
}
