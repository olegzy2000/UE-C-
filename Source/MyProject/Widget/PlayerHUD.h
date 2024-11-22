// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* OxygenProgressBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
		UReticleWidget* ReticleWidget;
	UPROPERTY(meta = (BindWidget))
		UAmmoWidget* AmmoWidget;
public:
	class UProgressBar* GetHealthProgressBar();
	class UProgressBar* GetOxygenProgressBar();
	class UProgressBar* GetStaminaProgressBar();
	UReticleWidget* GetReticleWidget();
	UAmmoWidget* GetAmmoWidget();
	
};
