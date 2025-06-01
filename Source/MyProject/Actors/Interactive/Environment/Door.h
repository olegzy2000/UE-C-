#pragma once
#include "CoreMinimal.h"
#include "../../../Characters/GCBaseCharacter.h"
#include "../Interactive.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class MYPROJECT_API ADoor : public AActor , public IInteractable{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADoor();

	virtual void Interact(AGCBaseCharacter* Character) override;

	virtual FName GetActionEventName() const override;
	virtual void Tick(float DeltaTime) override;

	virtual bool HasOnInteractionCallback() const;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	IInteractable::FOnInteration OnInteractionEvent;
private:	
	void InteractWithDoor();

	UFUNCTION()
		void UpdateDoorAnimation(float Alpha);
	UFUNCTION()
		void OnDoorAnimationFinished();
	FTimeline DoorOpenAnimTimeline;
	bool bIsOpened = false;
	

};
