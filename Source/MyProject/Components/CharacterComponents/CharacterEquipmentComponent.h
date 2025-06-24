// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include <Actors/Equipment/Throwable/ThrowableItem.h>
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include <Widget/Equipment/EquipmentViewWidget.h>
#include "Characters/GCBaseCharacter.h"
#include "CharacterEquipmentComponent.generated.h"
typedef TArray<AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterEquipmentComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	EEquipableItemType GetCurrentEquippedWeaponType() const;
	ARangeWeaponItem* GetCurrentRangeWeaponItem() const;
	void ReloadCurrentWeapon() ;
	void EquipItemInSlot(EEquipmentSlots Slot);
	void AttachCurrentItemToEquippedSocket();
	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;
	void UnEquipCurrentItem();
	void EquipNextItem();
	void EquipPreviousItem();
	bool IsEquipping() const;
	void LaunchCurrentThrowableItem();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo=0, bool bCheckIsFull=false);
	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChanged;
	FOnEquippedItemChanged OnEquippedItemChanged;
	bool AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem>EquipableItemClass, int32 SlotIndex);
	void RemoveItemFromSlot(int32 SlotIndex);
	void OpenViewEquipment(APlayerController* PlayerController);
	void CloseViewEquipment();
	bool IsViewVisible() const;
	const TArray<AEquipableItem*> GetItems() const;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TMap<EAmunitionType, int32>MaxAmunitionAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TMap<EEquipmentSlots, TSubclassOf<AEquipableItem>>ItemsLodout;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TSet<EEquipmentSlots>IgnoreSlotsWhileSwitching;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
		EEquipmentSlots AutoEquipItemInSlot=EEquipmentSlots::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
		TSubclassOf<UEquipmentViewWidget> ViewWidgetClass ;
	void CreateViewWidget(APlayerController* PlayerController);
private:
	void AutoEquip();
	bool bIsEquipping=false;
	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);
	AEquipableItem* CurrentEquippedItem;
	EEquipmentSlots CurrentEquippedSlot;
	EEquipmentSlots PreviosEquippedSlot;
	int32 GetAvailableAmunitionForCurrentWeapon();
	TAmunitionArray AmunitionArray;
	TArray<AEquipableItem*> ItemsArray;
	void CreateLoadout();
	ARangeWeaponItem* CurrentEquippedWeapon;
	AThrowableItem* CurrentThowableItem;
	AMeleeWeaponItem* CurrentMeleeeWeaponItem;
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
	UEquipmentViewWidget* ViewWidget;

};
