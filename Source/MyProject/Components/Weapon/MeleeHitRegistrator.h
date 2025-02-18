// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistrator.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMeleeHitRegistred, const FHitResult&, Hit, const FVector&, Vector);
UCLASS(meta=(BlueprintSpawnableComponent))
class MYPROJECT_API UMeleeHitRegistrator : public USphereComponent
{
	GENERATED_BODY()
public:
	UMeleeHitRegistrator();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ProcessHitRegistration();
	void SetIsHitRegistrationEnabled(bool bIsEnabled_In);
	FOnMeleeHitRegistred OnMeleeHitRegistred;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee hit registration")
		bool bIsHitRegistrationEnabled = false;
private:
	FVector PreviousComponentLocation;
};
