// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Widget/ProgressBarWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameModeBase.h"
#include "PayerGameModeBaseSecondVersion.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API APayerGameModeBaseSecondVersion : public AGameModeBase
{
	GENERATED_BODY()
		virtual void BeginPlay() override;

};
