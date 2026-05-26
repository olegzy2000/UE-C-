// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "CharacterInventoryComponent.h"
#include "../../Inventory/Items/Ammo/UInventoryAmmoItem.h"
#include "../../Inventory/InventoryItem.h"
#include <Characters/PlayerCharacter.h>
#include <Utils/GCSpawner.h>
#include <Utils/GCDataTableUtils.h>
bool UCharacterEquipmentComponent::AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex, int32 StartedAmmo)
{
	if (!ItemsArray.IsValidIndex(SlotIndex) || !IsValid(EquipableItemClass)) {
		return false;
	}
	AEquipableItem* EquipableItem = EquipableItemClass->GetDefaultObject<AEquipableItem>();
	if (!IsValid(EquipableItem)) {
		return false;
	}
	if (!EquipableItem->IsSlotCompatable((EEquipmentSlots)SlotIndex)) {
		return false;
	}
	if (!IsValid(ItemsArray[SlotIndex])) {
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(EquipableItemClass);
		if (!IsValid(Item) || !CachedBaseCharacter.IsValid() || !IsValid(CachedBaseCharacter->GetMesh())) {
			return false;
		}
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->SetAmmo(StartedAmmo);
		Item->UnEquip();
		ItemsArray[SlotIndex] = Item;
	}
	else if (EquipableItem->IsA<ARangeWeaponItem>()) {
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(EquipableItem);
		UCharacterInventoryComponent* InventoryComponent = CachedBaseCharacter->GetCharacterInventoryComponent();
		if (IsValid(InventoryComponent)) {
			InventoryComponent->AddAmmo(RangeWeaponObject->GetAmmoType(), RangeWeaponObject->GetMaxAmmo());
		}
	}
	return true;
}
void UCharacterEquipmentComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if ((uint32)CurrentEquippedSlot == SlotIndex) {
		UnEquipCurrentItem();
	}
	UE_LOG(LogTemp, Log, TEXT("UCharacterEquipmentComponent::RemoveItemFromSlot %i"), SlotIndex);
	ItemsArray[SlotIndex]->RemoveFromRoot();
	ItemsArray[SlotIndex]->Destroy();
	ItemsArray[SlotIndex] = nullptr;
}
const TArray<AEquipableItem*> UCharacterEquipmentComponent::GetItems() const
{
	return ItemsArray;
}
void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() UCharacterEquipmentComponent can be used only with AGCBaseCharacter"))
		CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());
	CreateLoadout();
	AutoEquip();
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEquipItemInSlot != EEquipmentSlots::None) {
		EquipItemInSlot(AutoEquipItemInSlot);
	}
}
uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1) {
		return 0;
	}
	return CurrentSlotIndex + 1;
}
uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0) {
		return ItemsArray.Num() - 1;
	}
	return CurrentSlotIndex - 1;
}
int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	check(IsValid(GetCurrentRangeWeaponItem()))
		UCharacterInventoryComponent* InventoryComponent = CachedBaseCharacter->GetCharacterInventoryComponent();
	return IsValid(InventoryComponent) ? InventoryComponent->GetAmmoAmount(GetCurrentRangeWeaponItem()->GetAmmoType()) : 0;
}
void UCharacterEquipmentComponent::CreateLoadout()
{
	UCharacterInventoryComponent* InventoryComponent = CachedBaseCharacter->GetCharacterInventoryComponent();
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount) {
		int32 LoadoutValue = FMath::Max(AmmoPair.Value, 0);
		if (LoadoutValue > 0 && IsValid(InventoryComponent)) {
			InventoryComponent->AddAmmo(AmmoPair.Key, LoadoutValue);
		}
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair <EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLodout) {
		if (!IsValid(ItemPair.Value)) {
			continue;
		}
		AddEquipmentItemToSlot(ItemPair.Value, (int32)ItemPair.Key, 0);
	}
}
void UCharacterEquipmentComponent::OnLevelDeserialized_Implementation()
{
	
	RebuildRuntimeEquipmentFromSaveData();

	if (CurrentEquippedSlot == EEquipmentSlots::None || !ItemsArray.IsValidIndex((int32)CurrentEquippedSlot)) {
		return;
	}

	EquipItemInSlot(CurrentEquippedSlot);
}
void UCharacterEquipmentComponent::OnCurrentWeaponChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChanged.IsBound()) {
		OnCurrentWeaponAmmoChanged.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon());
	}
}
void UCharacterEquipmentComponent::OnCurrentItemChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChanged.IsBound()) {
		OnCurrentWeaponAmmoChanged.Broadcast(Ammo, CurrentEquippedItem->GetMaxAmmo());
	}
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}
void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetCurrentAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);
	if (NumberOfAmmo > 0) {
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	UCharacterInventoryComponent* InventoryComponent = CachedBaseCharacter->GetCharacterInventoryComponent();
	ReloadedAmmo = IsValid(InventoryComponent) ? InventoryComponent->ConsumeAmmo(CurrentEquippedWeapon->GetAmmoType(), ReloadedAmmo) : 0;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);
	if (bCheckIsFull) {
		AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetCurrentAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloaded) {
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}


void UCharacterEquipmentComponent::Serialize(FArchive& Archive)
{
	if (Archive.IsSaveGame() && Archive.IsSaving()) {
		CaptureEquipmentSaveData();
	}

	Super::Serialize(Archive);

	if (Archive.IsSaveGame() && Archive.IsLoading()) {
		RestoreEquipmentSaveData();
	}
}

void UCharacterEquipmentComponent::CaptureEquipmentSaveData()
{
	EquipmentSaveData.Empty();
	SavedCurrentEquippedSlot = CurrentEquippedSlot;

	for (int32 SlotIndex = 0; SlotIndex < ItemsArray.Num(); ++SlotIndex) {
		AEquipableItem* Item = ItemsArray[SlotIndex];
		if (!IsValid(Item)) {
			continue;
		}

		FEquipmentSlotSaveData SlotSaveData;
		SlotSaveData.Slot = (EEquipmentSlots)SlotIndex;
		SlotSaveData.ItemId = Item->GetDataTableID();
		SlotSaveData.ItemClass = Item->GetClass();
		SlotSaveData.AmmoInMagazine = Item->GetCurrentAmmo();
		EquipmentSaveData.Add(SlotSaveData);
	}
}

void UCharacterEquipmentComponent::ClearRuntimeEquipment()
{
	UnEquipCurrentItem();

	for (AEquipableItem* Item : ItemsArray) {
		if (IsValid(Item)) {
			Item->Destroy();
		}
	}

	ItemsArray.Empty();
	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	CurrentEquippedItem = nullptr;
	CurrentEquippedWeapon = nullptr;
	CurrentThowableItem = nullptr;
	CurrentMeleeeWeaponItem = nullptr;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

TSubclassOf<AEquipableItem> UCharacterEquipmentComponent::ResolveSavedEquipmentClass(const FEquipmentSlotSaveData& SlotSaveData) const
{
	if (IsValid(SlotSaveData.ItemClass)) {
		return SlotSaveData.ItemClass;
	}

	if (SlotSaveData.ItemId.IsNone()) {
		return nullptr;
	}

	const FWeaponTableRow* WeaponData = GCDataTableUtils::FindWeaponData(SlotSaveData.ItemId);
	return WeaponData != nullptr ? WeaponData->EquipableActor : nullptr;
}

void UCharacterEquipmentComponent::RestoreEquipmentSaveData()
{
	if (!CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter = Cast<AGCBaseCharacter>(GetOwner());
	}

	if (!CachedBaseCharacter.IsValid() || !IsValid(GetWorld())) {
		return;
	}

	// Do not spawn equipment actors here. This function is called from Serialize()
	// while SaveSubsystem::DeserializeLevel may still be iterating Level->Actors.
	// Actors spawned at this point can be destroyed as "not found in save data".
	ClearRuntimeEquipment();
	CurrentEquippedSlot = SavedCurrentEquippedSlot;
}

void UCharacterEquipmentComponent::RebuildRuntimeEquipmentFromSaveData()
{
	if (!CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter = Cast<AGCBaseCharacter>(GetOwner());
	}

	if (!CachedBaseCharacter.IsValid() || !IsValid(GetWorld())) {
		return;
	}

	if (ItemsArray.Num() != (int32)EEquipmentSlots::MAX) {
		ItemsArray.Empty();
		ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	}

	for (const FEquipmentSlotSaveData& SlotSaveData : EquipmentSaveData) {
		const int32 SlotIndex = (int32)SlotSaveData.Slot;
		if (!ItemsArray.IsValidIndex(SlotIndex)) {
			UE_LOG(LogTemp, Warning, TEXT("UCharacterEquipmentComponent::RebuildRuntimeEquipmentFromSaveData skipped invalid slot %d"), SlotIndex);
			continue;
		}

		if (IsValid(ItemsArray[SlotIndex])) {
			continue;
		}

		const TSubclassOf<AEquipableItem> ResolvedItemClass = ResolveSavedEquipmentClass(SlotSaveData);
		if (!IsValid(ResolvedItemClass)) {
			UE_LOG(LogTemp, Warning, TEXT("UCharacterEquipmentComponent::RebuildRuntimeEquipmentFromSaveData failed to resolve item class for ItemId %s"), *SlotSaveData.ItemId.ToString());
			continue;
		}

		AddEquipmentItemToSlot(ResolvedItemClass, SlotIndex, SlotSaveData.AmmoInMagazine);
	}

	CurrentEquippedSlot = SavedCurrentEquippedSlot;
}

UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedWeaponType() const {
	EEquipableItemType Result = EEquipableItemType::None;
	if (CurrentEquippedItem && IsValid(CurrentEquippedItem)) {
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeaponItem() const
{
	return CurrentEquippedWeapon;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if (AvailableAmunition <= 0) {
		return;
	}
	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	//if (!IsValid(ItemsArray[(uint32)Slot])) {
	//	return;
	//}
	if (bIsEquipping) {
		return;
	}
	UnEquipCurrentItem();
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeeWeaponItem = Cast<AMeleeWeaponItem>(CurrentEquippedItem);
	if (IsValid(CurrentEquippedItem)) {
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage)) {
			bIsEquipping = true;
			UAnimInstance* CharacterAnimInstnce = CachedBaseCharacter->GetMesh()->GetAnimInstance();
			float EquipDuration = CharacterAnimInstnce->Montage_Play(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else {
			AttachCurrentItemToEquippedSocket();
		}
		CurrentEquippedSlot = Slot;
		CurrentEquippedItem->Equip();
	}
	if (IsValid(CurrentEquippedWeapon)) {
		OnCurrentWeaponAmmoChangeHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponChanged"));
		OnCurrentWeaponAReloadedHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponChanged(CurrentEquippedWeapon->GetCurrentAmmo());
	}
	else if (IsValid(CurrentThowableItem)) {
		OnCurrentItemChanged(CurrentThowableItem->GetCurrentAmmo());
	}
	if (OnEquippedItemChanged.IsBound()) {
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}
}

void UCharacterEquipmentComponent::StartLaunching(UAnimMontage* EquipMontage)
{
	if (IsValid(CurrentThowableItem)) {
		if (CurrentThowableItem->GetMaxAmmo() > 0) {
			bIsEquipping = true;
			UAnimInstance* CharacterAnimInstnce = CachedBaseCharacter->GetMesh()->GetAnimInstance();
			float EquipDuration = CharacterAnimInstnce->Montage_Play(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeapon() const
{
	return CurrentMeleeeWeaponItem;
}
AThrowableItem* UCharacterEquipmentComponent::GetCurrentThowableItem() const
{
	return CurrentThowableItem;
}
void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem)) {
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
	}
	if (IsValid(CurrentEquippedWeapon)) {
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangeHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponAReloadedHandle);
	}
	PreviosEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != NextSlotIndex
		&& (IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex)
			|| !IsValid(ItemsArray[NextSlotIndex]))) {
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex) {
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}

}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32  PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != PreviousSlotIndex
		&& (IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex)
			|| !IsValid(ItemsArray[PreviousSlotIndex]))) {
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex) {
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (IsValid(CurrentThowableItem)) {
		if (CurrentThowableItem->GetMaxAmmo() == 0)
			CurrentThowableItem->SetAmmo(0);
		else
			CurrentThowableItem->SetMaxAmmo(CurrentThowableItem->GetMaxAmmo() - 1);

		CurrentThowableItem->Throw();
		bIsEquipping = false;
		OnCurrentItemChanged(CurrentThowableItem->GetCurrentAmmo());
		//if (PreviosEquippedSlot == EEquipmentSlots::PrivaryItemSlot)
		//	PreviosEquippedSlot = EEquipmentSlots::None;
		//EquipItemInSlot(PreviosEquippedSlot);
	}
}


