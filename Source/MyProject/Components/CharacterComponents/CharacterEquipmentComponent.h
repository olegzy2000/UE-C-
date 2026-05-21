// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include "../../Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include <Actors/Equipment/Throwable/ThrowableItem.h>
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Characters/GCBaseCharacter.h"
#include "CharacterEquipmentComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

USTRUCT()
struct FEquipmentSlotSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	EEquipmentSlots Slot = EEquipmentSlots::None;

	UPROPERTY(SaveGame)
	FName ItemId = NAME_None;

	UPROPERTY(SaveGame)
	TSubclassOf<AEquipableItem> ItemClass;

	UPROPERTY(SaveGame)
	int32 AmmoInMagazine = 0;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()
public:
	UCharacterEquipmentComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	virtual void Serialize(FArchive& Archive) override;
	EEquipableItemType GetCurrentEquippedWeaponType() const;
	ARangeWeaponItem* GetCurrentRangeWeaponItem() const;
	void ReloadCurrentWeapon();
	void EquipItemInSlot(EEquipmentSlots Slot);
	void StartLaunching(UAnimMontage* EquipMontage);
	void AttachCurrentItemToEquippedSocket();
	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;
	AThrowableItem* GetCurrentThowableItem() const;
	void UnEquipCurrentItem();
	void EquipNextItem();
	void EquipPreviousItem();
	bool IsEquipping() const;
	void LaunchCurrentThrowableItem();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);
	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChanged;
	FOnEquippedItemChanged OnEquippedItemChanged;
	bool AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem>EquipableItemClass, int32 SlotIndex, int32 StartedAmmo);
	void RemoveItemFromSlot(int32 SlotIndex);
	const TArray<AEquipableItem*> GetItems() const;

	virtual void OnLevelDeserialized_Implementation() override;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32>MaxAmunitionAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<AEquipableItem>>ItemsLodout;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots>IgnoreSlotsWhileSwitching;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;
private:
	void AutoEquip();
	bool bIsEquipping = false;
	void CaptureEquipmentSaveData();
	void RestoreEquipmentSaveData();
	void RebuildRuntimeEquipmentFromSaveData();
	TSubclassOf<AEquipableItem> ResolveSavedEquipmentClass(const FEquipmentSlotSaveData& SlotSaveData) const;
	void ClearRuntimeEquipment();
	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);
	UPROPERTY(Transient)
	AEquipableItem* CurrentEquippedItem = nullptr;
	UPROPERTY(Transient)
	EEquipmentSlots CurrentEquippedSlot = EEquipmentSlots::None;
	EEquipmentSlots PreviosEquippedSlot;
	int32 GetAvailableAmunitionForCurrentWeapon();
	UPROPERTY(Transient)
	TArray<AEquipableItem*> ItemsArray;
	UPROPERTY(SaveGame)
	TArray<FEquipmentSlotSaveData> EquipmentSaveData;
	UPROPERTY(SaveGame)
	EEquipmentSlots SavedCurrentEquippedSlot = EEquipmentSlots::None;
	void CreateLoadout();
	UPROPERTY(Transient)
	ARangeWeaponItem* CurrentEquippedWeapon = nullptr;
	AThrowableItem* CurrentThowableItem = nullptr;
	AMeleeWeaponItem* CurrentMeleeeWeaponItem = nullptr;
	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;
	FTimerHandle EquipTimer;
	UFUNCTION()
	void OnCurrentWeaponChanged(int32 Ammo);
	void OnCurrentItemChanged(int32 Ammo);
	UFUNCTION()
	void OnWeaponReloadComplete();
	FDelegateHandle OnCurrentWeaponAmmoChangeHandle;
	FDelegateHandle OnCurrentWeaponAReloadedHandle;
	void EquipAnimationFinished();

};
