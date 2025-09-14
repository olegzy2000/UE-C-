// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_CharacterCrouch.h"
#include <Characters/GCBaseCharacter.h>

void UGameplayAbility_CharacterCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(OwnerInfo->AvatarActor.Get());
	BaseCharacter->GetBaseCharacterMovementComponent()->ChangeCrouchState();
}

void UGameplayAbility_CharacterCrouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UGameplayAbility_CharacterCrouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(ActorInfo->AvatarActor.Get());
	BaseCharacter->GetBaseCharacterMovementComponent()->ChangeCrouchState();
}
bool UGameplayAbility_CharacterCrouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const {
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) {
		return false;
	}
	if (ActorInfo->AvatarActor->IsA<AGCBaseCharacter>()) {
		AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(ActorInfo->AvatarActor.Get());
		return BaseCharacter->CanCrouch();
	}
	return false;
}
