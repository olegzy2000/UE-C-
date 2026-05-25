// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Interactive/Interactive.h"
#include "CharacterInteractionComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName)

class AGCBaseCharacter;
class AInteractiveActor;
class ALadder;
class AZipline;
class UGCBaseCharacterMovementComponent;
class APlayerController;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterInteractionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void Interact();
	void InteractionWithLadder();
	void InteractionWithZipline();
	void ClimbLadderUp(float Value);

	const ALadder* GetAvailableLadder() const;
	AZipline* GetAvailableZipline() const;

	FOnInteractableObjectFound OnInteractableObjectFound;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Interactive")
	float LineSightDistance = 500.0f;

private:
	void TraceOfSight();
	APlayerController* GetCachedPlayerController();

	UPROPERTY()
	TWeakObjectPtr<AGCBaseCharacter> CachedBaseCharacter;

	UPROPERTY()
	TWeakObjectPtr<UGCBaseCharacterMovementComponent> CachedBaseCharacterMovementComponent;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPlayerController;

	UPROPERTY()
	TScriptInterface<IInteractable> LineOfSightObject;

	UPROPERTY()
	TArray<AInteractiveActor*> AvailableInteractiveActors;
};
