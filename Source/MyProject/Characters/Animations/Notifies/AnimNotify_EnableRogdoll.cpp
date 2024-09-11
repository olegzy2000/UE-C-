// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableRogdoll.h"
#include <MyProject/GameCodeTypes.h>
void UAnimNotify_EnableRogdoll::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	MeshComponent->SetCollisionProfileName(CollisionProfileRagdoll);
	MeshComponent->SetSimulatePhysics(true);
}
