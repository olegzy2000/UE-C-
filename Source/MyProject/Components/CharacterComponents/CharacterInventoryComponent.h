// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class UInventoryItem;
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterInventoryComponent();
	void OpenViewInventory(APlayerController* Controller);
	void CloseViewInventory();
	bool IsViewVisible();
	int32 GetCapacity() const;
	bool HasFreeSlot();
	TArray<FInventorySlot>GetAllItemsCopy() const;
	TArray<FText>GetAllItemsNames() const;
	bool AddItem(TWeakObjectPtr<UInventoryItem>ItemToAdd, int32 Count);
	bool RemoveItem(FName ItemID);
protected:
	// Called when the game starts
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FInventorySlot> InventorySlots;
	UPROPERTY(EditAnywhere, Category = "View settings")
	TSubclassOf<class UInventoryViewWidget> InventoryViewWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory settings",Category="Inventory settings")
	int32 Capacity = 16;
	void CreateViewWidget(APlayerController* PlayerController);
	FInventorySlot* FindItemSlot(FName ItemID);
	FInventorySlot* FindFreeSlot();
private:
	UPROPERTY()
	UInventoryViewWidget* InventoryViewWidget;

	int32 ItemsInInventory;
		
};
