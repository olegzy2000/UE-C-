// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamingSubsystemUtils.h"
#include "GameFramework/Character.h"
#include "StreamingSubsystemManager.h"
#include "StreamingSubsystemVolum.h"
void UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(ACharacter* Character)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(): %s"), *GetNameSafe(Character));
	if (!IsValid(Character)) {
		return;
	}
	TSet<AActor*> OverlappingActors;
	Character->GetOverlappingActors(OverlappingActors, AStreamingSubsystemVolum::StaticClass());
	for (AActor* OverlappedActor : OverlappingActors) {
		if (!IsValid(OverlappedActor) || !OverlappedActor->HasActorBegunPlay()) {
			continue;
		}
		StaticCast<AStreamingSubsystemVolum*>(OverlappedActor)->HandleCharacterOverlapBegin(Character);
	}

}

void UStreamingSubsystemUtils::CheckStreamingSubsystemVolumeOverlapCharacter(AStreamingSubsystemVolum* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystemUtils::CheckStreamingSubsystemVolumeOverlapCharacter(): %s"), *GetNameSafe(SubsystemVolume));
	if (!IsValid(SubsystemVolume)) {
		return;
	}
	TSet<AActor*> OverlappingActors;
	SubsystemVolume->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
	for (AActor* OverlappedActor : OverlappingActors) {
		if (!IsValid(OverlappedActor) || !OverlappedActor->HasActorBegunPlay()) {
			continue;
		}
		SubsystemVolume->HandleCharacterOverlapBegin(StaticCast<ACharacter*>(OverlappedActor));
	}
}
