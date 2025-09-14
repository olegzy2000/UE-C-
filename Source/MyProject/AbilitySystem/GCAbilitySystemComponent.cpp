// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GCAbilitySystemComponent.h"

bool UGCAbilitySystemComponent::TryActivateAbilityWithTag(FGameplayTag GameplayTag, bool AllowRemoteActiovation)
{
	return TryActivateAbilitiesByTag(FGameplayTagContainer(GameplayTag), AllowRemoteActiovation);
}

bool UGCAbilitySystemComponent::TryCancelAbilityWithTag(FGameplayTag GameplayTag)
{
	bool Result = false;
	TArray<FGameplayAbilitySpec*>AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(GameplayTag), AbilitiesToActivate, false);
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate) {
		TArray<UGameplayAbility*>AbilityInstances = Spec->GetAbilityInstances();
		for (UGameplayAbility* ActiveAbility : AbilityInstances) {
			ActiveAbility->K2_CancelAbility();
			Result = true;
		}
	}
	return Result;
}

bool UGCAbilitySystemComponent::IsAbilityActive(FGameplayTag GameplayTag) const
{
	bool Result = false;
	TArray<FGameplayAbilitySpec*>AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(GameplayTag), AbilitiesToActivate, false);
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate) {
		TArray<UGameplayAbility*>AbilityInstances = Spec->GetAbilityInstances();
		for (UGameplayAbility* ActiveAbility : AbilityInstances) {
			Result |=ActiveAbility->IsActive();
		}
	}
	return Result;
}
