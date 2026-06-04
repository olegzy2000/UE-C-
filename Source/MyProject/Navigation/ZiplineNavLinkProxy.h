// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "ZiplineNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AZiplineNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	AZiplineNavLinkProxy();
	UFUNCTION()
	void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);

};
