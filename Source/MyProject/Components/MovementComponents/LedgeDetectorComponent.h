// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Platformes/BasePlatform.h"
#include "Math/Rotator.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
		FVector Location;

	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
		FVector LedgeNormal;


	    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
		FRotator Rotation;
		UPROPERTY()
		ABasePlatform* HitObject;
	    
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
		float MinimumLedgeHeight = 40.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
		float MaximumLedgeHeight = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
		float ForwardCheckDistance = 100.0f;


public:
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);
private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;


};
