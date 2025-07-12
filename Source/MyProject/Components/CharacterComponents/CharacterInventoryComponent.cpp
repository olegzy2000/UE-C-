// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"
#include "../../Widget/Inventory/InventoryViewWidget.h"
#include "../../Inventary/InventoryItem.h"
#include "../../Utils/GCSpawner.h"
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
	BaseCharacterOwner = Cast<AGCBaseCharacter>(GetOwner());
	//BaseCharacterOwner->GetCharacterEquipmentComponent_Mutable()->OnCurrentWeaponAmmoChanged.AddUFunction(this, FName("UpdateInventoryAmmoComponentAmount"));
	InventorySlots.AddDefaulted(Capacity);
}
void UCharacterInventoryComponent::UpdateInventoryAmmoComponentAmount() {
	EAmunitionType AmmoType=BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetAmmoType();
	int32 Amount = BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetCurrentAmmo();
	UpdateAmountAmmoInSlot(AmmoType, Amount);
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
		//Result = UpdateInventoryAmmoSlotByWeaponAmmo(ItemToAdd);
	}
	if (ItemToAdd->IsA<UInventoryAmmoItem>()) {
		UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(ItemToAdd);
		Result = UpdateAmountAmmoInSlot(CurrentInventoryAmmoItem->GetAmmoType(), CurrentInventoryAmmoItem->GetAmount());
		if(!Result)
			Result = CreateNewInventorySlot(ItemToAdd, Count);
		UpdateAmunition(CurrentInventoryAmmoItem->GetAmmoType(), CurrentInventoryAmmoItem->GetAmount());
	}
	if(!Result) {
		Result=CreateNewInventorySlot(ItemToAdd, Count);
	}
	return Result;
}

bool UCharacterInventoryComponent::CreateNewInventorySlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, const int32 Count)
{
	bool Result = false;
	FInventorySlot* FreeSlot = FindFreeSlot();
	if (FreeSlot != nullptr) {
		FreeSlot->Item = ItemToAdd;
		FreeSlot->Count = Count;
		ItemsInInventory++;
		Result = true;
		FreeSlot->UpdateSlotState();
	}
	return Result;
}

bool UCharacterInventoryComponent::UpdateAmountAmmoInSlot(EAmunitionType AmunitionType,int32 Amount)
{
	bool Result = false;
	FInventorySlot* InventorySlot = FindSlotWithCustomAmmoItem(AmunitionType);
	if (InventorySlot != nullptr) {
		UInventoryAmmoItem* InventoryAmmoItemInInventory = Cast<UInventoryAmmoItem>(InventorySlot->Item);
		int32 ResultAmount = InventoryAmmoItemInInventory->GetAmount() + Amount;
		InventoryAmmoItemInInventory->SetAmount(ResultAmount);
		InventorySlot->UpdateSlotState();
		Result = true;
	}
	return Result;
}
void UCharacterInventoryComponent::UpdateAmunition(EAmunitionType AmunitionType, const int32& Amount)
{
	BaseCharacterOwner->GetCharacterEquipmentComponent_Mutable()->AddAmunition(AmunitionType, Amount);
}
// TO REMOVE
bool UCharacterInventoryComponent::UpdateInventoryAmmoSlotByWeaponAmmo(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	UWeaponInventoryItem* WeaponInventoryItem = Cast<UWeaponInventoryItem>(ItemToAdd);
	AEquipableItem* EquipableItem = WeaponInventoryItem->GetEquipWeaponClass()->GetDefaultObject<AEquipableItem>();
	if (EquipableItem->IsA<ARangeWeaponItem>()) {
	  ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(EquipableItem);
	  FName AmmoType=NAME_None;
	  switch (RangeWeaponObject->GetAmmoType())
	  {
	  case EAmunitionType::Pistol :{
		  AmmoType= FName(TEXT("Pistol"));
		  break;
	  }
	  case EAmunitionType::Rifle: {
		  AmmoType = FName(TEXT("Rifle"));
		  break;
	  }
	  case EAmunitionType::ShotgunShells: {
		  AmmoType = FName(TEXT("ShotgunShells"));
		  break;
	  }
	  case EAmunitionType::Sniper: {
		  AmmoType = FName(TEXT("Sniper"));
		  break;
	  }
	  default:
		  break;
	  }
	  if (!AmmoType.IsNone()) {
		  TWeakObjectPtr<UInventoryAmmoItem> AmmoItem = GCSpawner::SpawnInventoryAmmoItem(Cast<AGCBaseCharacter>(GetOwner()), AmmoType, RangeWeaponObject->GetMaxAmmo());
		  AddItem(AmmoItem, 1);
	  }
	  //AmunitionArray[SlotIndex] += RangeWeaponObject->GetMaxAmmo();
	}
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
