// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Components/Weapon/MeleeHitRegistrator.h"
AMeleeWeaponItem::AMeleeWeaponItem() {
	EquppedSocketName = SocketCharacterWeapon;
}
void AMeleeWeaponItem::StartAttack(EMeleeAttackTypes AttackType)
{
	AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();

	if (!IsValid(CurrentCharacterOwner)) {
		return;
	}
	HitActors.Empty();
	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack && IsValid(CurrentAttack->AttackMontage)) {
		UAnimInstance* CharacterAnimInstance = CurrentCharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance)) {
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackTimerElapsed, Duration,false);
		}
		else {
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeaponItem::SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled)
{
	HitActors.Empty();
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators) {
		HitRegistrator->SetIsHitRegistrationEnabled(bIsRegistrationEnabled);
	}
}

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistrators);
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators) {
		HitRegistrator->OnMeleeHitRegistred.AddDynamic(this, &AMeleeWeaponItem::ProcessHit);
	}
}

void AMeleeWeaponItem::ProcessHit(const FHitResult& HitResult, const FVector& HitDirection)
{
	if (CurrentAttack == nullptr) {
		return;
	}
	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor)) {
		return;
	}
	if (HitActors.Contains(HitActor)) {
		return;
	}
	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = HitDirection;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageTypeClass;
	AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
	AController* Controller = CurrentCharacterOwner->GetController();
	HitActor->TakeDamage(CurrentAttack->DamageAmount , DamageEvent, Controller, GetOwner());
	HitActors.Add(HitActor);
}

void AMeleeWeaponItem::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
	HitActors.Empty();
	SetIsHitRegistrationEnabled(false);
}