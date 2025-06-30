// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"
class UImage;
struct FInventorySlot;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateView();
	void InitializeItemSlot(FInventorySlot& InventarySlot);
	void SetItemIcon(UTexture2D* Icon);
	void SetAmount(int32 Amount);
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ImageItemIcon;
	UPROPERTY(meta = (BindWidget))
	int32 Amount;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:
	FInventorySlot* LinkedSlot;
};
