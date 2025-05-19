// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GCBaseCharacter.h"
#include "GCAICharacter.generated.h"

/**
 * 
 */
class UBehaviorTree;
class UAIPatrollingComponent;
UCLASS(Blueprintable)
class MYPROJECT_API AGCAICharacter : public AGCBaseCharacter
{
	GENERATED_BODY()
public:
	AGCAICharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Mantle(bool bForce) override;
	UAIPatrollingComponent* GetPatrollingComponent() const;
	UBehaviorTree* GetUBehaviorTree() const;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrollingComponent* AIPatrollingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
