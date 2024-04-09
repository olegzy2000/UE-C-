// Fill out your copyright notice in the Description page of Project Settings.


#include "FatigueBar.h"

void UFatigueBar::NativeConstruct()
{
	Super::NativeConstruct();
}

UProgressBar* UFatigueBar::GetFatigueBar()
{
	return FatigueBar;
}
