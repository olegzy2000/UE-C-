// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"
#include "../../Widget/Inventory/InventoryViewWidget.h"
#include "../../Inventary/InventoryItem.h"
#include <Characters/PlayerCharacter.h>
#include <Inventary/Items/Ammo/UInventoryAmmoItem.h>
#include <Widget/Equipment/EquipmentSlotWidget.h>
#include "../../Inventary/Items/Equipables/WeaponInventoryItem.h"
// Sets default values for this component's properties
UCharacterInventoryComponent::UCharacterInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}
void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
// Called when the game starts
void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InventorySlots.AddDefaulted(Capacity);
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* PlayerController)
{
	if (IsValid(InventoryViewWidget)) {
		return;
	}
	if (!IsValid(PlayerController) || !IsValid(InventoryViewWidgetClass)) {
		return;
	}
	InventoryViewWidget = CreateWidget<UInventoryViewWidget>(PlayerController, InventoryViewWidgetClass);
	InventoryViewWidget->SetVisibility(ESlateVisibility::Visible);
	InventoryViewWidget->InitializeViewWidget(InventorySlots);

}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot){ return Slot.Item->GetDataTableID()==ItemID; });
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return !Slot.Item.IsValid(); });
}

FInventorySlot* UCharacterInventoryComponent::FindSlotWithCustomAmmoItem(EAmunitionType AmmoType)
{
	FInventorySlot* InventorySlotResult=InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {
		TWeakObjectPtr<UInventoryItem> CurrentInventoryItem = Slot.Item;
		if (CurrentInventoryItem.IsValid() && CurrentInventoryItem->IsA<UInventoryAmmoItem>()) {
			UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(CurrentInventoryItem);
			if (CurrentInventoryAmmoItem->GetAmmoType() == AmmoType) {
				return true;
			}
		}
		return false;
	});
	return InventorySlotResult;
}


void FInventorySlot::BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const
{
	OnInventorySlotUpdate = Callback;
}

void FInventorySlot::UnbindOnInventorySlotUpdate()
{
	OnInventorySlotUpdate.Unbind();
}

void FInventorySlot::UpdateSlotState()
{
	OnInventorySlotUpdate.ExecuteIfBound();
}

void FInventorySlot::ClearSlot()
{
	Item = nullptr;
	Count = 0;
	UpdateSlotState();
}

void UCharacterInventoryComponent::OpenViewInventory(APlayerController* Controller)
{
	if (!IsValid(InventoryViewWidget)) {
		CreateViewWidget(Controller);
	}
	//if (InventoryViewWidget->IsVisible()) {
		InventoryViewWidget->AddToViewport();
	//}

}

void UCharacterInventoryComponent::CloseViewInventory()
{
	if (InventoryViewWidget->IsVisible()) {
		InventoryViewWidget->RemoveFromParent();
	}
}

bool UCharacterInventoryComponent::IsViewVisible()
{
	bool Result = false;
	if (IsValid(InventoryViewWidget)) {
		Result = InventoryViewWidget->IsVisible();
	}
	return Result;
}

int32 UCharacterInventoryComponent::GetCapacity() const
{
	return Capacity;
}

bool UCharacterInventoryComponent::HasFreeSlot()
{
	return ItemsInInventory<Capacity;
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText>Result;
	for (const FInventorySlot& Slot : InventorySlots) {
		if (Slot.Item.IsValid()) {
			Result.Add(Slot.Item->GetDescription().Name);
		}
	}
	return Result;
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count)
{
	if (!ItemToAdd.IsValid() || Count<0) {
		return false;
	}
	bool Result = false;
	if (ItemToAdd->IsA<UWeaponInventoryItem>()) {
		Result = UpdateInventoryAmmoSlotByWeaponAmmo(ItemToAdd);
	}
	if (ItemToAdd->IsA<UInventoryAmmoItem>()) {
		Result = UpdateAmountAmmoInSlot(ItemToAdd);
	}
	if(!Result) {
		FInventorySlot* FreeSlot = FindFreeSlot();
		if (FreeSlot != nullptr) {
			FreeSlot->Item = ItemToAdd;
			FreeSlot->Count = Count;
			ItemsInInventory++;
			Result = true;
			FreeSlot->UpdateSlotState();
		}
	}
	return Result;
}

bool UCharacterInventoryComponent::UpdateAmountAmmoInSlot(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	bool Result = false;
	UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(ItemToAdd);
	FInventorySlot* InventorySlot = FindSlotWithCustomAmmoItem(CurrentInventoryAmmoItem->GetAmmoType());
	if (InventorySlot != nullptr) {
		UInventoryAmmoItem* InventoryAmmoItemInInventory = Cast<UInventoryAmmoItem>(InventorySlot->Item);
		int32 ResultAmount = CurrentInventoryAmmoItem->GetAmount() + InventoryAmmoItemInInventory->GetAmount();
		InventoryAmmoItemInInventory->SetAmount(ResultAmount);
		InventorySlot->UpdateSlotState();
		Result = true;
	}
	return Result;
}

bool UCharacterInventoryComponent::UpdateInventoryAmmoSlotByWeaponAmmo(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	return false;
}

bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* ItemSlot = FindItemSlot(ItemID);
	if (ItemSlot != nullptr) {
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableID() == ItemID; });
		return true;
	}
	return false;
}
