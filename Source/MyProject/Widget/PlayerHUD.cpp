// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/PlayerHUD.h"

#include "Components/ProgressBar.h"
#include "HighglightInteractable.h"

void UPlayerHUD::SetHealthPercent(float Percent)
{
    if (IsValid(HealthProgressBar))
    {
        HealthProgressBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
    }
}

void UPlayerHUD::SetStaminaPercent(float Percent)
{
    if (IsValid(StaminaProgressBar))
    {
        StaminaProgressBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
    }
}

void UPlayerHUD::SetOxygenPercent(float Percent)
{
    if (IsValid(OxygenProgressBar))
    {
        const float ClampedPercent = FMath::Clamp(Percent, 0.0f, 1.0f);
        OxygenProgressBar->SetPercent(ClampedPercent);
        OxygenProgressBar->SetVisibility(ClampedPercent < 1.0f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UPlayerHUD::SetHealthBarColor(const FLinearColor& Color)
{
    if (IsValid(HealthProgressBar))
    {
        HealthProgressBar->SetFillColorAndOpacity(Color);
    }
}

void UPlayerHUD::SetStaminaBarColor(const FLinearColor& Color)
{
    if (IsValid(StaminaProgressBar))
    {
        StaminaProgressBar->SetFillColorAndOpacity(Color);
    }
}

void UPlayerHUD::SetOxygenBarColor(const FLinearColor& Color)
{
    if (IsValid(OxygenProgressBar))
    {
        OxygenProgressBar->SetFillColorAndOpacity(Color);
    }
}

UReticleWidget* UPlayerHUD::GetReticleWidget()
{
    return ReticleWidget;
}

UAmmoWidget* UPlayerHUD::GetAmmoWidget()
{
    return AmmoWidget;
}

void UPlayerHUD::SetHightInteractableActionText(FName KeyName)
{
    if (IsValid(InteractableKey))
    {
        InteractableKey->SetActionText(KeyName);
    }
}

void UPlayerHUD::SetHighlightInteractableVisibility(bool bIsVisible)
{
    if (!IsValid(InteractableKey))
    {
        return;
    }

    InteractableKey->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    SetHealthPercent(1.0f);
    SetStaminaPercent(1.0f);
    SetOxygenPercent(1.0f);
}
