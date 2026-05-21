// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameCodeTypes.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "CharacterInventoryComponent.generated.h"

class UInventoryItem;
class AGCBaseCharacter;

UENUM()
enum class EInventorySlotSaveType : uint8
{
	None,
	GenericItem,
	WeaponItem,
	AmmoItem
};

USTRUCT()
struct FInventorySlotSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FName ItemId = NAME_None;

	UPROPERTY(SaveGame)
	int32 Count = 0;

	UPROPERTY(SaveGame)
	EInventorySlotSaveType ItemType = EInventorySlotSaveType::None;

	UPROPERTY(SaveGame)
	EAmunitionType AmmoType = EAmunitionType::None;

	UPROPERTY(SaveGame)
	int32 AmmoAmount = 0;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE(FInventorySlotUpdate)
	UPROPERTY(EDitAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	void BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const;
	void UnbindOnInventorySlotUpdate();
	void UpdateSlotState();
	void ClearSlot();
private:
	mutable FInventorySlotUpdate OnInventorySlotUpdate;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterInventoryComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	UCharacterInventoryComponent();
	int32 GetCapacity() const;
	bool HasFreeSlot();
	TArray<FInventorySlot>GetAllItemsCopy() const;
	TArray<FInventorySlot>& GetInventorySlots_Mutable();
	TArray<FText>GetAllItemsNames() const;
	bool AddItem(TWeakObjectPtr<UInventoryItem>ItemToAdd, int32 Count);
	bool CreateNewInventorySlot(const TWeakObjectPtr<UInventoryItem> ItemToAdd, const int32 Count);
	int32 GetAmmoAmount(EAmunitionType AmunitionType) const;
	bool AddAmmo(EAmunitionType AmunitionType, int32 Amount);
	int32 ConsumeAmmo(EAmunitionType AmunitionType, int32 Amount);
	bool UpdateAmountAmmoInSlot(EAmunitionType AmunitionType, int32 Amount);
	bool UpdateInventoryAmmoSlotByWeaponAmmo(TWeakObjectPtr<UInventoryItem> ItemToAdd);
	bool RemoveItem(FName ItemID);
	virtual void OnLevelDeserialized_Implementation() override;
protected:
	// Called when the game starts
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void Serialize(FArchive& Archive) override;

	void UpdateInventoryAmmoComponentAmount();

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FInventorySlot> InventorySlots;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory settings", Category = "Inventory settings")
	int32 Capacity = 16;
	FInventorySlot* FindItemSlot(FName ItemID);
	FInventorySlot* FindFreeSlot();
	FInventorySlot* FindSlotWithCustomAmmoItem(EAmunitionType AmmoType);
	void CaptureInventorySaveData();
	void RestoreInventorySaveData();

	UPROPERTY(SaveGame)
	TArray<FInventorySlotSaveData> InventorySaveData;
private:
	AGCBaseCharacter* BaseCharacterOwner;
	int32 ItemsInInventory;

};
