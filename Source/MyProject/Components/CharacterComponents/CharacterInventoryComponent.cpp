// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"
#include "../../Inventory/InventoryItem.h"
#include "CharacterEquipmentComponent.h"
#include "../../Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "../../Utils/GCSpawner.h"
#include <Characters/PlayerCharacter.h>
#include <Inventory/Items/Ammo/UInventoryAmmoItem.h>
#include <Widget/Equipment/EquipmentSlotWidget.h>
#include "../../Inventory/Items/Equipables/WeaponInventoryItem.h"
#include <Utils/GCDataTableUtils.h>

namespace
{
	FName GetAmmoDataTableId(EAmunitionType AmunitionType)
	{
		switch (AmunitionType)
		{
		case EAmunitionType::Pistol:
			return FName(TEXT("Pistol"));
		case EAmunitionType::Rifle:
			return FName(TEXT("Rifle"));
		case EAmunitionType::ShotgunShells:
			return FName(TEXT("ShotgunShells"));
		case EAmunitionType::Sniper:
			return FName(TEXT("Sniper"));
		default:
			return NAME_None;
		}
	}

	EInventorySlotSaveType ResolveItemType(const UInventoryItem* Item)
	{
		if (!IsValid(Item)) {
			return EInventorySlotSaveType::None;
		}
		if (Item->IsA<UInventoryAmmoItem>()) {
			return EInventorySlotSaveType::AmmoItem;
		}
		if (Item->IsA<UWeaponInventoryItem>()) {
			return EInventorySlotSaveType::WeaponItem;
		}
		return EInventorySlotSaveType::GenericItem;
	}

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
UCharacterInventoryComponent::UCharacterInventoryComponent()
	: ItemsInInventory(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterInventoryComponent::Serialize(FArchive& Archive)
{
	if (Archive.IsSaveGame() && Archive.IsSaving()) {
		CaptureInventorySaveData();
	}

	Super::Serialize(Archive);

	if (Archive.IsSaveGame() && Archive.IsLoading()) {
		RestoreInventorySaveData();
	}
}

void UCharacterInventoryComponent::CaptureInventorySaveData()
{
	InventorySaveData.Empty();

	for (const FInventorySlot& Slot : InventorySlots) {
		if (Slot.IsEmpty()) {
			continue;
		}

		FInventorySlotSaveData SlotSaveData;
		SlotSaveData.ItemId = Slot.GetItemId();
		SlotSaveData.Count = Slot.GetCount();
		SlotSaveData.ItemType = Slot.GetItemType();
		SlotSaveData.AmmoType = Slot.GetAmmoType();
		SlotSaveData.AmmoAmount = Slot.GetAmmoAmount();
		SlotSaveData.WeaponAmmoInMagazine = Slot.GetWeaponAmmoInMagazine();

		InventorySaveData.Add(SlotSaveData);
	}
}

void UCharacterInventoryComponent::RestoreInventorySaveData()
{
	InventorySlots.Empty();
	InventorySlots.AddDefaulted(Capacity);
	ItemsInInventory = 0;

	for (const FInventorySlotSaveData& SlotSaveData : InventorySaveData) {
		if (SlotSaveData.ItemId.IsNone() || SlotSaveData.ItemType == EInventorySlotSaveType::None) {
			continue;
		}

		CreateNewInventorySlotFromData(
			SlotSaveData.ItemId,
			SlotSaveData.Count,
			SlotSaveData.ItemType,
			SlotSaveData.AmmoType,
			SlotSaveData.AmmoAmount,
			SlotSaveData.WeaponAmmoInMagazine
		);
	}
}

void UCharacterInventoryComponent::OnLevelDeserialized_Implementation()
{
	// Inventory state is restored during Serialize() from InventorySaveData.
}

// Called when the game starts
void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseCharacterOwner = Cast<AGCBaseCharacter>(GetOwner());
	//BaseCharacterOwner->GetCharacterEquipmentComponent_Mutable()->OnCurrentWeaponAmmoChanged.AddUFunction(this, FName("UpdateInventoryAmmoComponentAmount"));
	if (InventorySlots.Num() == 0) {
		InventorySlots.AddDefaulted(Capacity);
	}
}

void UCharacterInventoryComponent::UpdateInventoryAmmoComponentAmount()
{
	if (!IsValid(BaseCharacterOwner) || !IsValid(BaseCharacterOwner->GetCharacterEquipmentComponent()) || !IsValid(BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem())) {
		return;
	}
	EAmunitionType AmmoType = BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetAmmoType();
	int32 Amount = BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetCurrentAmmo();
	UpdateAmountAmmoInSlot(AmmoType, Amount);
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return Slot.GetItemId() == ItemID; });
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return Slot.IsEmpty(); });
}

FInventorySlot* UCharacterInventoryComponent::FindSlotWithCustomAmmoItem(EAmunitionType AmmoType)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {
		return Slot.GetItemType() == EInventorySlotSaveType::AmmoItem && Slot.GetAmmoType() == AmmoType;
		});
}

bool FInventorySlot::IsEmpty() const
{
	return ItemId.IsNone() || ItemType == EInventorySlotSaveType::None;
}

FName FInventorySlot::GetItemId() const
{
	return ItemId;
}

int32 FInventorySlot::GetCount() const
{
	return Count;
}

EInventorySlotSaveType FInventorySlot::GetItemType() const
{
	return ItemType;
}

EAmunitionType FInventorySlot::GetAmmoType() const
{
	return AmmoType;
}

int32 FInventorySlot::GetAmmoAmount() const
{
	return AmmoAmount;
}

int32 FInventorySlot::GetWeaponAmmoInMagazine() const
{
	return WeaponAmmoInMagazine;
}

void FInventorySlot::SetSlotData(FName NewItemId, int32 NewCount, EInventorySlotSaveType NewItemType, EAmunitionType NewAmmoType, int32 NewAmmoAmount, int32 NewWeaponAmmoInMagazine)
{
	ItemId = NewItemId;
	ItemType = NewItemType;
	Count = NewItemId.IsNone() || NewItemType == EInventorySlotSaveType::None ? 0 : FMath::Max(NewCount, 1);
	AmmoType = NewItemType == EInventorySlotSaveType::AmmoItem ? NewAmmoType : EAmunitionType::None;
	AmmoAmount = NewItemType == EInventorySlotSaveType::AmmoItem ? FMath::Max(NewAmmoAmount, 0) : 0;
	WeaponAmmoInMagazine = NewItemType == EInventorySlotSaveType::WeaponItem ? FMath::Max(NewWeaponAmmoInMagazine, 0) : 0;
	UpdateSlotState();
}

void FInventorySlot::SetSlotItem(TWeakObjectPtr<UInventoryItem> ItemToSet, int32 NewCount)
{
	if (!ItemToSet.IsValid()) {
		ClearSlot();
		return;
	}

	const EInventorySlotSaveType NewItemType = ResolveItemType(ItemToSet.Get());
	EAmunitionType NewAmmoType = EAmunitionType::None;
	int32 NewAmmoAmount = 0;
	int32 NewWeaponAmmoInMagazine = 0;

	if (const UInventoryAmmoItem* AmmoItem = Cast<UInventoryAmmoItem>(ItemToSet.Get())) {
		NewAmmoType = AmmoItem->GetAmmoType();
		NewAmmoAmount = AmmoItem->GetAmount();
	}
	else if (const UWeaponInventoryItem* WeaponItem = Cast<UWeaponInventoryItem>(ItemToSet.Get())) {
		NewWeaponAmmoInMagazine = WeaponItem->GetStartedAmmoAmount();
	}

	SetSlotData(ItemToSet->GetDataTableID(), NewCount, NewItemType, NewAmmoType, NewAmmoAmount, NewWeaponAmmoInMagazine);
}

TWeakObjectPtr<UInventoryItem> FInventorySlot::CreateRuntimeItem(AGCBaseCharacter* Owner) const
{
	if (!IsValid(Owner) || IsEmpty()) {
		return nullptr;
	}

	switch (ItemType)
	{
	case EInventorySlotSaveType::AmmoItem:
		return GCSpawner::SpawnInventoryAmmoItem(Owner, ItemId, AmmoAmount);
	case EInventorySlotSaveType::WeaponItem:
	{
		TWeakObjectPtr<UWeaponInventoryItem> WeaponItem = GCSpawner::SpawnInventoryWeaponItem(Owner, ItemId);
		if (WeaponItem.IsValid()) {
			WeaponItem->SetStartedAmmoAmount(WeaponAmmoInMagazine);
		}
		return WeaponItem;
	}
	case EInventorySlotSaveType::GenericItem:
		return GCSpawner::SpawnInventoryItem(Owner, ItemId);
	default:
		return nullptr;
	}
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
	ItemId = NAME_None;
	Count = 0;
	ItemType = EInventorySlotSaveType::None;
	AmmoType = EAmunitionType::None;
	AmmoAmount = 0;
	WeaponAmmoInMagazine = 0;
	UpdateSlotState();
}

int32 UCharacterInventoryComponent::GetCapacity() const
{
	return Capacity;
}

bool UCharacterInventoryComponent::HasFreeSlot()
{
	return ItemsInInventory < Capacity;
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FInventorySlot>& UCharacterInventoryComponent::GetInventorySlots_Mutable()
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText> Result;
	for (const FInventorySlot& Slot : InventorySlots) {
		FInventoryItemDescription Description;
		if (ResolveSlotDescription(Slot, Description)) {
			Result.Add(Description.Name);
		}
	}
	return Result;
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count)
{
	if (!ItemToAdd.IsValid() || Count < 0) {
		return false;
	}

	if (ItemToAdd->IsA<UInventoryAmmoItem>()) {
		UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(ItemToAdd);
		return AddAmmo(CurrentInventoryAmmoItem->GetAmmoType(), CurrentInventoryAmmoItem->GetAmount());
	}

	return CreateNewInventorySlot(ItemToAdd, Count);
}

bool UCharacterInventoryComponent::CreateNewInventorySlot(TWeakObjectPtr<UInventoryItem> ItemToAdd, const int32 Count)
{
	if (!ItemToAdd.IsValid()) {
		return false;
	}

	const EInventorySlotSaveType ItemType = ResolveItemType(ItemToAdd.Get());
	EAmunitionType AmmoType = EAmunitionType::None;
	int32 AmmoAmount = 0;

	if (const UInventoryAmmoItem* AmmoItem = Cast<UInventoryAmmoItem>(ItemToAdd.Get())) {
		AmmoType = AmmoItem->GetAmmoType();
		AmmoAmount = AmmoItem->GetAmount();
	}

	int32 WeaponAmmoInMagazine = 0;
	if (const UWeaponInventoryItem* WeaponItem = Cast<UWeaponInventoryItem>(ItemToAdd.Get())) {
		WeaponAmmoInMagazine = WeaponItem->GetStartedAmmoAmount();
	}

	return CreateNewInventorySlotFromData(ItemToAdd->GetDataTableID(), Count, ItemType, AmmoType, AmmoAmount, WeaponAmmoInMagazine);
}

bool UCharacterInventoryComponent::CreateNewInventorySlotFromData(FName ItemId, int32 Count, EInventorySlotSaveType ItemType, EAmunitionType AmmoType, int32 AmmoAmount, int32 WeaponAmmoInMagazine)
{
	FInventorySlot* FreeSlot = FindFreeSlot();
	if (FreeSlot == nullptr || ItemId.IsNone() || ItemType == EInventorySlotSaveType::None) {
		return false;
	}

	FreeSlot->SetSlotData(ItemId, Count, ItemType, AmmoType, AmmoAmount, WeaponAmmoInMagazine);
	ItemsInInventory++;
	return true;
}

int32 UCharacterInventoryComponent::GetAmmoAmount(EAmunitionType AmunitionType) const
{
	for (const FInventorySlot& Slot : InventorySlots) {
		if (Slot.GetItemType() == EInventorySlotSaveType::AmmoItem && Slot.GetAmmoType() == AmunitionType) {
			return Slot.GetAmmoAmount();
		}
	}

	return 0;
}

bool UCharacterInventoryComponent::AddAmmo(EAmunitionType AmunitionType, int32 Amount)
{
	if (Amount <= 0) {
		return false;
	}

	if (UpdateAmountAmmoInSlot(AmunitionType, Amount)) {
		return true;
	}

	const FName AmmoDataTableId = GetAmmoDataTableId(AmunitionType);
	if (AmmoDataTableId.IsNone()) {
		return false;
	}

	return CreateNewInventorySlotFromData(AmmoDataTableId, 1, EInventorySlotSaveType::AmmoItem, AmunitionType, Amount);
}

int32 UCharacterInventoryComponent::ConsumeAmmo(EAmunitionType AmunitionType, int32 Amount)
{
	if (Amount <= 0) {
		return 0;
	}

	const int32 AvailableAmmo = GetAmmoAmount(AmunitionType);
	const int32 ConsumedAmmo = FMath::Min(AvailableAmmo, Amount);
	if (ConsumedAmmo > 0) {
		UpdateAmountAmmoInSlot(AmunitionType, -ConsumedAmmo);
	}

	return ConsumedAmmo;
}

bool UCharacterInventoryComponent::UpdateAmountAmmoInSlot(EAmunitionType AmunitionType, int32 Amount)
{
	FInventorySlot* InventorySlot = FindSlotWithCustomAmmoItem(AmunitionType);
	if (InventorySlot == nullptr) {
		return false;
	}

	const int32 ResultAmount = FMath::Max(InventorySlot->GetAmmoAmount() + Amount, 0);
	InventorySlot->AmmoAmount = ResultAmount;
	InventorySlot->Count = FMath::Max(InventorySlot->Count, 1);
	InventorySlot->UpdateSlotState();
	return true;
}

// TO REMOVE
bool UCharacterInventoryComponent::UpdateInventoryAmmoSlotByWeaponAmmo(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	UWeaponInventoryItem* WeaponInventoryItem = Cast<UWeaponInventoryItem>(ItemToAdd);
	if (!IsValid(WeaponInventoryItem) || !IsValid(WeaponInventoryItem->GetEquipWeaponClass())) {
		return false;
	}

	AEquipableItem* EquipableItem = WeaponInventoryItem->GetEquipWeaponClass()->GetDefaultObject<AEquipableItem>();
	if (EquipableItem->IsA<ARangeWeaponItem>()) {
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(EquipableItem);
		AddAmmo(RangeWeaponObject->GetAmmoType(), RangeWeaponObject->GetMaxAmmo());
	}
	return false;
}

bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* ItemSlot = FindItemSlot(ItemID);
	if (ItemSlot != nullptr) {
		ItemSlot->ClearSlot();
		ItemsInInventory = FMath::Max(ItemsInInventory - 1, 0);
		return true;
	}
	return false;
}
