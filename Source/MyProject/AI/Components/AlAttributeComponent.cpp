// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/AlAttributeComponent.h"
#include "../../GameCodeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "../../Subsystems/DebugSubsystem.h"
#include <Runtime/Engine/Public/DrawDebugHelpers.h>
#include "AI/Turrent/Turret.h"
UAlAttributeComponent::UAlAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UAlAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ATurret>(), TEXT("UAlAttributeComponent::BeginPlay UCharacterAttributeComponent can be used only with ATurret"));
	CachedTurretOwner = StaticCast<ATurret*>(GetOwner());
}
void UAlAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

void UAlAttributeComponent::SetHealth(float NewHealth)
{
	this->Health = NewHealth;
}
float UAlAttributeComponent::GetHealth()
{
	return this->Health;
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UAlAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (DebugSubsystem->IsCategoryEnable(DebugCategoryCharacterAttributes)) {
		return;
	}
	FVector TextLocation = CachedTurretOwner->GetActorLocation();
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Red, 0.0f, true);
}
#endif