// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCAIController.h"
#include "MyGCAICharacterController.generated.h"

/**
 * 
 */
class AGCAICharacter;
UCLASS()
class MYPROJECT_API AMyAGCAICharacterController : public AGCAIController
{
	GENERATED_BODY()
/*public:
	virtual void SetPawn(APawn* InPawn) override;
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)override;
private:
	TWeakObjectPtr<AGCAICharacter>CachedAICharacter;
	bool bIsPatrolling;
protected:
	virtual void BeginPlay() override;*/
};
