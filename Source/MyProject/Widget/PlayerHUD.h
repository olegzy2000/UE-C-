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
class UHighglightInteractable;
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
	UPROPERTY(meta = (BindWidget))
	UHighglightInteractable* InteractableKey;

public:
	UFUNCTION()
	void SetHealthPercent(float Percent);

	UFUNCTION()
	void SetStaminaPercent(float Percent);

	UFUNCTION()
	void SetOxygenPercent(float Percent);

	void SetHealthBarColor(const FLinearColor& Color);
	void SetStaminaBarColor(const FLinearColor& Color);
	void SetOxygenBarColor(const FLinearColor& Color);

	class UProgressBar* GetHealthProgressBar();
	class UProgressBar* GetOxygenProgressBar();
	class UProgressBar* GetStaminaProgressBar();
	UReticleWidget* GetReticleWidget();
	UAmmoWidget* GetAmmoWidget();

	void SetHightInteractableActionText(FName KeyName);
	void SetHighlightInteractableVisibility(bool bIsVisible);
};
