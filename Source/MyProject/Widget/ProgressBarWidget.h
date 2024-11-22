// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar;
public:
	class UProgressBar* GetProgressBar();

};
