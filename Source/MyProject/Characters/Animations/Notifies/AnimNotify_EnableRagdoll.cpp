// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableRagdoll.h"
#include <MyProject/GameCodeTypes.h>
void UAnimNotify_EnableRagdoll::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	MeshComponent->SetCollisionProfileName(CollisionProfileRagdoll);
	MeshComponent->SetSimulatePhysics(true);
}
