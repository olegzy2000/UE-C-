// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FatigueBar.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UFatigueBar : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* FatigueBar;
public:
	UProgressBar* GetFatigueBar();
private:
};
