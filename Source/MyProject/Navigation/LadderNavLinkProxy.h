// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "LadderNavLinkProxy.generated.h"

class AGCAICharacter;
class ALadder;

UCLASS()
class MYPROJECT_API ALadderNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	ALadderNavLinkProxy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);

	void ClimbTick(float DeltaTime);
	void StartClimbTick();
	void StopClimbTick();
	void FinishLadderTraversal();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Ladder")
	ALadder* TargetLadder = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Ladder")
	bool bClimbUp = true;

private:
	TWeakObjectPtr<AGCAICharacter> PendingAICharacter;
};