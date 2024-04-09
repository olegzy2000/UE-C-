#pragma once
#include "../PlatformInvocator.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlatformInvocatorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlatformInvocatorClient);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UPlatformInvocatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlatformInvocatorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditInstanceOnly)
	APlatformInvocator* PlatformInvocator;
	UPROPERTY(BlueprintAssignable, Category = "Invokator client")
	FPlatformInvocatorClient InvocatorClient;

	UFUNCTION()
	void Invoke();

		
};
