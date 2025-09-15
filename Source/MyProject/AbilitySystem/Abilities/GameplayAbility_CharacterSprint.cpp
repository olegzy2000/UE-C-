// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_CharacterSprint.h"
#include <Characters/GCBaseCharacter.h>


void UGameplayAbility_CharacterSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);
	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(OwnerInfo->AvatarActor.Get());
	BaseCharacter->GetBaseCharacterMovementComponent()->StartSprint();
}

void UGameplayAbility_CharacterSprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UGameplayAbility_CharacterSprint::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AGCBaseCharacter* BaseCharacter = StaticCast<AGCBaseCharacter*>(ActorInfo->AvatarActor.Get());
	BaseCharacter->GetBaseCharacterMovementComponent()->StopSprint();
}
bool UGameplayAbility_CharacterSprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags , const FGameplayTagContainer* TargetTags , OUT FGameplayTagContainer* OptionalRelevantTags) const {
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) {
		return false;
	}
	return ActorInfo->AvatarActor->IsA<AGCBaseCharacter>();
}
