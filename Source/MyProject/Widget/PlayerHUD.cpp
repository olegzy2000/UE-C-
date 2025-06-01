// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerHUD.h"
#include "HighglightInteractable.h"
UProgressBar* UPlayerHUD::GetStaminaProgressBar()
{
    return StaminaProgressBar;
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
    if (IsValid(InteractableKey)) {
        InteractableKey->SetActionText(KeyName);
    }
}
void UPlayerHUD::SetHighlightInteractableVisibility(bool bIsVisible)
{
    if (!IsValid(InteractableKey)) {
        return;
    }
    if (bIsVisible) {
        InteractableKey->SetVisibility(ESlateVisibility::Visible);
    }
    else {
        InteractableKey->SetVisibility(ESlateVisibility::Hidden);
    }
}
UProgressBar* UPlayerHUD::GetOxygenProgressBar()
{
    return OxygenProgressBar;
}
UProgressBar* UPlayerHUD::GetHealthProgressBar()
{
    return HealthProgressBar;
}
void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();
}
