// Fill out your copyright notice in the Description page of Project Settings.


#include "HighglightInteractable.h"
void UHighglightInteractable::SetActionText(FName KeyName) {
	if (IsValid(ActionText)) {
		ActionText->SetText(FText::FromName(KeyName));
	}
}
