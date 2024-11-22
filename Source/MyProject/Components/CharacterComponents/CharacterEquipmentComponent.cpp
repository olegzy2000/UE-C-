// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() UCharacterEquipmentComponent can be used only with AGCBaseCharacter"))
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());

	CreateLoadout();
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
	return AmunitionArray[(uint32)GetCurrentRangeWeaponItem()->GetAmmoType()];
}
void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount) {
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair <EEquipmentSlots, TSubclassOf<AEquipableItem>>&ItemPair:ItemsLodout) {
		if (!IsValid(ItemPair.Value)) {
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,Item->GetUnEquppedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[(uint32)ItemPair.Key]=Item;
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChanged.IsBound()) {
		OnCurrentWeaponAmmoChanged.Broadcast(Ammo,AmunitionArray[(uint32)GetCurrentRangeWeaponItem()->GetAmmoType()]);
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
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);
	if (NumberOfAmmo > 0) {
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);
	if (bCheckIsFull) {
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
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

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedWeaponType() const {
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedWeapon)) {
		Result = CurrentEquippedWeapon->GetItemType();
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
	if (AvailableAmunition <=0) {
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
	if (IsValid(CurrentEquippedItem)) {
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage)) {
			if (IsValid(CurrentThowableItem)) {
				if (CurrentThowableItem->GetAmmo() > 0) {
					bIsEquipping = true;
					UAnimInstance* CharacterAnimInstnce = CachedBaseCharacter->GetMesh()->GetAnimInstance();
					float EquipDuration = CharacterAnimInstnce->Montage_Play(EquipMontage);
					GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
				}
				else {

				}
			}
			else {
				bIsEquipping = true;
				UAnimInstance* CharacterAnimInstnce = CachedBaseCharacter->GetMesh()->GetAnimInstance();
				float EquipDuration = CharacterAnimInstnce->Montage_Play(EquipMontage);
				GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
			}
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
		OnCurrentWeaponChanged(CurrentEquippedWeapon->GetAmmo());
	}
	else if (IsValid(CurrentThowableItem)) {
		OnCurrentItemChanged(CurrentThowableItem->GetAmmo());
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if(IsValid(CurrentEquippedItem))
	CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquppedSocketName());
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem)) {
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquppedSocketName());
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
	if (CurrentSlotIndex!=NextSlotIndex) {
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
		    OnCurrentItemChanged(CurrentThowableItem->GetAmmo());
			if (PreviosEquippedSlot == EEquipmentSlots::PrivaryItemSlot)
				PreviosEquippedSlot = EEquipmentSlots::None;
			EquipItemInSlot(PreviosEquippedSlot);
		
	}
}


