// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"
#include "../../Inventary/InventoryItem.h"
#include "CharacterEquipmentComponent.h"
#include "../../Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "../../Utils/GCSpawner.h"
#include <Characters/PlayerCharacter.h>
#include <Inventary/Items/Ammo/UInventoryAmmoItem.h>
#include <Widget/Equipment/EquipmentSlotWidget.h>
#include "../../Inventary/Items/Equipables/WeaponInventoryItem.h"
// Sets default values for this component's properties
UCharacterInventoryComponent::UCharacterInventoryComponent()
	: ItemsInInventory(0)
{
	PrimaryComponentTick.bCanEverTick = true;
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
		if (!Slot.Item.IsValid()) {
			continue;
		}

		FInventorySlotSaveData SlotSaveData;
		SlotSaveData.ItemId = Slot.Item->GetDataTableID();
		SlotSaveData.Count = Slot.Count;

		if (const UInventoryAmmoItem* AmmoItem = Cast<UInventoryAmmoItem>(Slot.Item.Get())) {
			SlotSaveData.ItemType = EInventorySlotSaveType::AmmoItem;
			SlotSaveData.AmmoType = AmmoItem->GetAmmoType();
			SlotSaveData.AmmoAmount = AmmoItem->GetAmount();
		}
		else if (Slot.Item->IsA<UWeaponInventoryItem>()) {
			SlotSaveData.ItemType = EInventorySlotSaveType::WeaponItem;
		}
		else {
			SlotSaveData.ItemType = EInventorySlotSaveType::GenericItem;
		}

		InventorySaveData.Add(SlotSaveData);
	}
}

void UCharacterInventoryComponent::RestoreInventorySaveData()
{
	if (!IsValid(BaseCharacterOwner)) {
		BaseCharacterOwner = Cast<AGCBaseCharacter>(GetOwner());
	}

	InventorySlots.Empty();
	InventorySlots.AddDefaulted(Capacity);
	ItemsInInventory = 0;

	if (!IsValid(BaseCharacterOwner)) {
		return;
	}

	for (const FInventorySlotSaveData& SlotSaveData : InventorySaveData) {
		if (SlotSaveData.ItemId.IsNone() || SlotSaveData.ItemType == EInventorySlotSaveType::None) {
			continue;
		}

		TWeakObjectPtr<UInventoryItem> RestoredItem = nullptr;
		int32 RestoredCount = SlotSaveData.Count;

		if (SlotSaveData.ItemType == EInventorySlotSaveType::AmmoItem) {
			RestoredItem = GCSpawner::SpawnInventoryAmmoItem(BaseCharacterOwner, SlotSaveData.ItemId, SlotSaveData.AmmoAmount);
			RestoredCount = FMath::Max(RestoredCount, 1);
		}
		else if (SlotSaveData.ItemType == EInventorySlotSaveType::WeaponItem) {
			RestoredItem = GCSpawner::SpawnInventoryWeaponItem(BaseCharacterOwner, SlotSaveData.ItemId);
		}
		else {
			RestoredItem = GCSpawner::SpawnInventoryItem(BaseCharacterOwner, SlotSaveData.ItemId);
		}

		if (RestoredItem.IsValid()) {
			CreateNewInventorySlot(RestoredItem, RestoredCount);
		}
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
void UCharacterInventoryComponent::UpdateInventoryAmmoComponentAmount() {
	EAmunitionType AmmoType = BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetAmmoType();
	int32 Amount = BaseCharacterOwner->GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem()->GetCurrentAmmo();
	UpdateAmountAmmoInSlot(AmmoType, Amount);
}
FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return Slot.Item->GetDataTableID() == ItemID; });
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) { return !Slot.Item.IsValid(); });
}

FInventorySlot* UCharacterInventoryComponent::FindSlotWithCustomAmmoItem(EAmunitionType AmmoType)
{
	FInventorySlot* InventorySlotResult = InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {
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
	if (!ItemToAdd.IsValid() || Count < 0) {
		return false;
	}

	if (ItemToAdd->IsA<UInventoryAmmoItem>()) {
		UInventoryAmmoItem* CurrentInventoryAmmoItem = Cast<UInventoryAmmoItem>(ItemToAdd);
		return AddAmmo(CurrentInventoryAmmoItem->GetAmmoType(), CurrentInventoryAmmoItem->GetAmount());
	}

	if (ItemToAdd->IsA<UWeaponInventoryItem>()) {
		//Result = UpdateInventoryAmmoSlotByWeaponAmmo(ItemToAdd);
	}

	return CreateNewInventorySlot(ItemToAdd, Count);
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
}

int32 UCharacterInventoryComponent::GetAmmoAmount(EAmunitionType AmunitionType) const
{
	for (const FInventorySlot& Slot : InventorySlots) {
		if (!Slot.Item.IsValid() || !Slot.Item->IsA<UInventoryAmmoItem>()) {
			continue;
		}

		const UInventoryAmmoItem* InventoryAmmoItem = Cast<UInventoryAmmoItem>(Slot.Item);
		if (InventoryAmmoItem && InventoryAmmoItem->GetAmmoType() == AmunitionType) {
			return InventoryAmmoItem->GetAmount();
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
	if (AmmoDataTableId.IsNone() || !IsValid(BaseCharacterOwner)) {
		return false;
	}

	TWeakObjectPtr<UInventoryAmmoItem> AmmoItem = GCSpawner::SpawnInventoryAmmoItem(BaseCharacterOwner, AmmoDataTableId, Amount);
	return AmmoItem.IsValid() && CreateNewInventorySlot(AmmoItem, 1);
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
	bool Result = false;
	FInventorySlot* InventorySlot = FindSlotWithCustomAmmoItem(AmunitionType);
	if (InventorySlot != nullptr) {
		UInventoryAmmoItem* InventoryAmmoItemInInventory = Cast<UInventoryAmmoItem>(InventorySlot->Item);
		int32 ResultAmount = FMath::Max(InventoryAmmoItemInInventory->GetAmount() + Amount, 0);
		InventoryAmmoItemInInventory->SetAmount(ResultAmount);
		InventorySlot->UpdateSlotState();
		Result = true;
	}
	return Result;
}
// TO REMOVE
bool UCharacterInventoryComponent::UpdateInventoryAmmoSlotByWeaponAmmo(TWeakObjectPtr<UInventoryItem> ItemToAdd)
{
	UWeaponInventoryItem* WeaponInventoryItem = Cast<UWeaponInventoryItem>(ItemToAdd);
	AEquipableItem* EquipableItem = WeaponInventoryItem->GetEquipWeaponClass()->GetDefaultObject<AEquipableItem>();
	if (EquipableItem->IsA<ARangeWeaponItem>()) {
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(EquipableItem);
		FName AmmoType = NAME_None;
		switch (RangeWeaponObject->GetAmmoType())
		{
		case EAmunitionType::Pistol: {
			AmmoType = FName(TEXT("Pistol"));
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
