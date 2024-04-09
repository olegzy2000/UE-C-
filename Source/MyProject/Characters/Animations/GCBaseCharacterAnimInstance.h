// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/WeakObjectPtrTemplates.h"
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "GCBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGCBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UGCBaseCharacterAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void setLeftEffectorLocation(FVector NewEffectorLocation);
	void setRightEffectorLocation(FVector NewEffectorLocation);
	FVector getLeftEffectorLocation();
	FVector getRightEffectorLocation();
	void SetLeftFootAlpha(float NewAlpha);
	void SetRightFootAlpha(float NewAlpha);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		float Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsFalling = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsCrouching = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsProning = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsSwimming = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsOnZipline = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsRunningOnWall = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsOnLadder = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		float LadderSpeedRation=0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		FVector LeftFootEffectorLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		FVector JoinTargetLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		float LeftFootAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		FVector RightFootEffectorLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		FVector JoinTargetRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | IK", meta = (BlueprintProtected = true))
		float RightFootAlpha;

private:
	UPROPERTY()
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
};
