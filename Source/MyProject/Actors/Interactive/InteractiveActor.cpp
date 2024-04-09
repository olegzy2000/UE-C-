// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include <MyProject/Characters/GCBaseCharacter.h>



void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(InteractionVolume)) {
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this,&AInteractiveActor::OnInterationVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInterationVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInterationVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(OtherActor);
	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInterationVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsOverlappingCharacterCapsule(OtherActor,OtherComp))
	{
		return;
	}
	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(OtherActor);
	BaseCharacter->UnRegisterInteractiveActor(this);
	
}

bool AInteractiveActor::IsOverlappingCharacterCapsule( AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return false;
	}
	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent()) {
		return false;
	}
	return true;
}

