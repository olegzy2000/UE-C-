// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterTraversalComponent.generated.h"

class AGCBaseCharacter;
class UGCBaseCharacterMovementComponent;
class ULedgeDetectorComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterTraversalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterTraversalComponent();

	virtual void BeginPlay() override;

	void Mantle(bool bForce);
	void TryToRunWall();
	bool CanMantle() const;

private:

	UPROPERTY()
	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;

	UPROPERTY()
	TWeakObjectPtr<UGCBaseCharacterMovementComponent> CachedBaseCharacterMovementComponent;

	UPROPERTY()
	TWeakObjectPtr<ULedgeDetectorComponent> CachedLedgeDetectorComponent;
};
