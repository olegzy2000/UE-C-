// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarWidget.h"


void UProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

UProgressBar* UProgressBarWidget::GetProgressBar()
{
	return ProgressBar;
}
