#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "TimerManager.h"
#include "GCBaseCharacter.h"
#include "PlayerCharacter.generated.h"

class AGCPlayerController;
class UCameraBehaviorComponent;
class UOxygenManagerComponent;
class UStaminaManagerComponent;
/**
 *
 */
UCLASS(Blueprintable)
class MYPROJECT_API APlayerCharacter : public AGCBaseCharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// 
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	//  
	virtual void ChangeProneState() override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnSprintEnd_Implementation() override;
	virtual void OnSprintStart_Implementation() override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;
	virtual void StartSprint() override;
	virtual void StopSprint() override;
	virtual void Jump() override;
	void Slide() override;

	//   
	virtual void StartAiming() override;
	virtual void StopAiming() override;
	virtual void StartFire() override;
	virtual void StopFire() override;

	// 
	virtual void SwimForward(float Value) override;
	virtual void SwimUp(float Value) override;
	virtual void SwimRight(float Value) override;

	// 
	void SwitchCameraPosition();

	//   
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	// 
	class UCameraBehaviorComponent* GetCameraBehaviorComponent() const { return CameraBehaviorComponent; }

protected:
	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

private:
	// 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraBehaviorComponent* CameraBehaviorComponent;

	//  
	UPROPERTY()
	AGCPlayerController* PlayerController;

	//  
	UPROPERTY(EditInstanceOnly, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float SpringArmLengthInSprint = 140.0f;

	UPROPERTY(EditInstanceOnly, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float DefaultSpringArmLength = 350.0f;



	//  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UOxygenManagerComponent* OxygenManagerComponent;

	UFUNCTION()
	void OnOxygenDepleted();

	UFUNCTION()
	void OnOxygenRestored();


	//       
	void CheckUnderwaterState(float DeltaTime);

	// 
	UPROPERTY()
	FTimerHandle StopAimTimerHandle;

	//  
	bool bIsCallingAimingByFire = false;

	bool bWasSwimmingLastFrame = false;


	// 

	// stamina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaminaManagerComponent* StaminaManagerComponent;

	//   
	UFUNCTION()
	void OnStaminaDepleted();

	UFUNCTION()
	void OnStaminaRestored();

};