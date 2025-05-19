// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/GCAIController.h"
#include "GCAICharacterController.generated.h"

/**
 * 
 */
class AGCAICharacter;


UCLASS()
class MYPROJECT_API AGCAICharacterController : public AGCAIController
{
	GENERATED_BODY()
public:
	virtual void SetPawn(APawn* InPawn)override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)override;
private:
	void TryMoveNextTarget();
	bool IsTargetReached(FVector TargetLocation);
	TWeakObjectPtr<AGCAICharacter> CachedAICharacter;
	bool bIsPatrolling = false;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float TargetReachRadius=100.0f;
	virtual void BeginPlay() override;
	
};
