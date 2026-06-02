#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "MantleNavLinkProxy.generated.h"
class AGCAICharacter;

UCLASS()
class MYPROJECT_API AMantleNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	AMantleNavLinkProxy();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);

	void RotateCharacterToSmartLinkDestination(
		AGCAICharacter* AICharacter,
		const FVector& DestinationPoint
	) const;

private:
	TWeakObjectPtr<AGCAICharacter> CachedAICharacter;
};