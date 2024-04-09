// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Widget/FatigueBar.h"
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
protected:
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="User interface | setting")
	    TSubclassOf<class UUserWidget> FatigueBar;
		UPROPERTY()
		UFatigueBar* CurrentWidget;
public:
	UFatigueBar* GetCurrentWidget();

};
