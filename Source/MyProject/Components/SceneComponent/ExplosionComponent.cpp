// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SceneComponent/ExplosionComponent.h"
#include "MyProject.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
void UExplosionComponent::Explode(AController* Controller) {
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogWeapon, Warning, TEXT("Explosion skipped: world is invalid | Component=%s"), *GetNameSafe(this));
		return;
	}

	TArray<AActor*>IgnoredActors;
	if (IsValid(GetOwner()))
	{
		IgnoredActors.Add(GetOwner());
	}
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetComponentLocation(),
		InnerRadius,
		OuterRadius,
		DamageFalloff,
		DamageTypeClass,
		IgnoredActors,
		GetOwner(),
		Controller,
		ECC_Visibility
	);
	if (IsValid(ExplosionVFX)) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
	if (OnExplosion.IsBound()) {
		OnExplosion.Broadcast();
	}
}