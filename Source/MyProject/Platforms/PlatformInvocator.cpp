// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"

APlatformInvocator::APlatformInvocator()
{
	PrimaryActorTick.bCanEverTick = false;

}

void APlatformInvocator::InvocePlatformToStart()
{
	InvocatorActivated.Broadcast();
}
