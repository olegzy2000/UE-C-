// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/MeleeHitRegistrator.h"
#include <GameCodeTypes.h>
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <Utils/GCTraceUtils.h>

void UMeleeHitRegistrator::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	if (bIsHitRegistrationEnabled) {
		ProcessHitRegistration();
	}
	PreviousComponentLocation = GetComponentLocation();
}

void UMeleeHitRegistrator::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubSystem->IsCategoryEnable(DebugCategoryMelee);
#else
	bool bIsDebugEnable = false;
#endif
	bool bHasHit = GCTraceUtils::SweepSphereSingleByChanel(
	    GetWorld(),
	    HitResult,
		PreviousComponentLocation,
		CurrentLocation,
		GetRelativeRotation().Quaternion(),
		ECC_Melee,
		GetScaledSphereRadius(),
		QueryParams,
		FCollisionResponseParams::DefaultResponseParam,
		bIsDebugEnable,
		5.0f
	);
	if (bHasHit) {
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		if (OnMeleeHitRegistred.IsBound()) {
			OnMeleeHitRegistred.Broadcast(HitResult,Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegistrationEnabled(bool bIsEnabled_In)
{
	bIsHitRegistrationEnabled = bIsEnabled_In;
}
UMeleeHitRegistrator::UMeleeHitRegistrator() {
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.0f;
	SetCollisionProfileName(CollisionProfileMelee);
}