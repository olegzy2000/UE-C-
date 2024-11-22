// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerHUD.h"

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
