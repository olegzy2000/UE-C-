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

	UPROPERTY(SaveGame)
	int32 WeaponAmmoInMagazine = 0;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE(FInventorySlotUpdate)

	// Step 4B: inventory slots no longer own runtime UInventoryItem objects.
	// The slot stores stable data only; temporary UInventoryItem objects are created
	// only for legacy UI drag/drop payloads or Consume() compatibility.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInventorySlotSaveType ItemType = EInventorySlotSaveType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmunitionType AmmoType = EAmunitionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmoInMagazine = 0;

	bool IsEmpty() const;
	FName GetItemId() const;
	int32 GetCount() const;
	EInventorySlotSaveType GetItemType() const;
	EAmunitionType GetAmmoType() const;
	int32 GetAmmoAmount() const;
	int32 GetWeaponAmmoInMagazine() const;
	void SetSlotData(FName NewItemId, int32 NewCount, EInventorySlotSaveType NewItemType, EAmunitionType NewAmmoType = EAmunitionType::None, int32 NewAmmoAmount = 0, int32 NewWeaponAmmoInMagazine = 0);
	void SetSlotItem(TWeakObjectPtr<UInventoryItem> ItemToSet, int32 NewCount);
	TWeakObjectPtr<UInventoryItem> CreateRuntimeItem(AGCBaseCharacter* Owner) const;
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
	bool CreateNewInventorySlotFromData(FName ItemId, int32 Count, EInventorySlotSaveType ItemType, EAmunitionType AmmoType = EAmunitionType::None, int32 AmmoAmount = 0, int32 WeaponAmmoInMagazine = 0);
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
