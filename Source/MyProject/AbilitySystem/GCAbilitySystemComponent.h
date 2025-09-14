// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GCAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	bool TryActivateAbilityWithTag(FGameplayTag GameplayTag , bool AllowRemoteActiovation = true);
	bool TryCancelAbilityWithTag(FGameplayTag GameplayTag);
	bool IsAbilityActive(FGameplayTag GameplayTag) const;
};
