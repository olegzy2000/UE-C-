// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <MyProject/GameCodeTypes.h>
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Components/CharacterComponents/CharacterInteractionComponent.h"
#include "Components/CharacterComponents/CharacterCombatComponent.h"
#include "Components/CharacterComponents/CharacterTraversalComponent.h"
#include "Components/CharacterComponents/CharacterFootIKComponent.h"
#include "../Inventory/InventoryItem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "AIController.h"


AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());
	LegDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventory"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
	CharacterInteractionComponent = CreateDefaultSubobject<UCharacterInteractionComponent>(TEXT("CharacterInteraction"));
	CharacterCombatComponent = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("CharacterCombat"));
	CharacterTraversalComponent = CreateDefaultSubobject<UCharacterTraversalComponent>(TEXT("CharacterTraversal"));
	CharacterFootIKComponent = CreateDefaultSubobject<UCharacterFootIKComponent>(TEXT("CharacterFootIK"));
}
void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GCBaseCharacterMovementComponent == nullptr) {
		GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());
		GCBaseCharacterMovementComponent->RotationRate.Pitch = 540.0f;
	}
	if (IsValid(GetMesh())) {
		InitialMeshRalativeLocation = GetMesh()->GetRelativeTransform().GetLocation();
	}
	InitializeHealthProgress();
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &AGCBaseCharacter::OnDeath);
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->OnInteractableObjectFound.BindLambda([this](FName ActionName) {
			OnInteractableObjectFound.ExecuteIfBound(ActionName);
			});
	}
}
void AGCBaseCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (OnInteractableObjectFound.IsBound()) {
		OnInteractableObjectFound.Unbind();
	}
	Super::EndPlay(Reason);
}
void AGCBaseCharacter::ChangeCrouchState()
{
	if (CanCrouch()) {
		GetBaseCharacterMovementComponent()->ChangeCrouchState();
		OnChangeCrouchState();
	}
}

void AGCBaseCharacter::OnChangeCrouchState()
{
	if (GetBaseCharacterMovementComponent()->IsProning()) {
		GetBaseCharacterMovementComponent()->ChangeProneState();
		ChangeCapsuleParamFromProneToCrouched();
		ChangeMaxSpeedOfPlayer(300.0f);
	}
	else {
		if (GetBaseCharacterMovementComponent()->IsCrouched()) {
			ChangeCapsuleParamFromIdleWalkStateToCrouch();
			ChangeMaxSpeedOfPlayer(300.0f);
		}
		else {
			ChangeCapsuleParamFromCrouchedToIdleWalk();
			ChangeMaxSpeedOfPlayer(600.0f);
		}
	}
}

bool AGCBaseCharacter::CanCrouch() const
{
	TArray<AActor*>ActorsToIgnore;
	FHitResult TraceHit;
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(this, GetCapsuleComponent()->GetRelativeLocation(),
		GetCapsuleComponent()->GetRelativeLocation() + FVector(0.0f, 0.0f, GetDefaultCapsuleHeight() + 45), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, TraceHit, true);
	return !bIsHit && !GetBaseCharacterMovementComponent()->IsFalling()
		&& !GetBaseCharacterMovementComponent()->IsSwimming()
		&& !GetBaseCharacterMovementComponent()->IsSlide()
		&& !GetBaseCharacterMovementComponent()->IsSprinting();
}

void AGCBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController)) {
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}
void AGCBaseCharacter::StartSprint()
{
	if (!GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSlide()) {
		bIsSprintRequested = true;
		if (GetBaseCharacterMovementComponent()->IsCrouched()) {
			ChangeCrouchState();
		}
	}
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AGCBaseCharacter::Slide()
{
	GetBaseCharacterMovementComponent()->TryToSlide();
}

void AGCBaseCharacter::StartFire()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->StartFire();
	}
}

bool AGCBaseCharacter::CanStartFire() {
	return IsValid(CharacterCombatComponent) && CharacterCombatComponent->CanStartFire();
}
void AGCBaseCharacter::ChangeMaxSpeedOfPlayer(float speed)
{
	GetBaseCharacterMovementComponent()->MaxWalkSpeed = speed;
}
void AGCBaseCharacter::StopFire()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->StopFire();
	}
}
void AGCBaseCharacter::StartAiming()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->StartAiming();
	}
}
void AGCBaseCharacter::StopAiming()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->StopAiming();
	}
}
void AGCBaseCharacter::PrimaryMeleeAttack()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->PrimaryMeleeAttack();
	}
}
void AGCBaseCharacter::SecondaryMeleeAttack()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->SecondaryMeleeAttack();
	}
}
void AGCBaseCharacter::EquipPrimaryItem()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->EquipPrimaryItem();
	}
}
float AGCBaseCharacter::GetAimingMovementSpeed() const
{
	return IsValid(CharacterCombatComponent) ? CharacterCombatComponent->GetAimingMovementSpeed() : 0.0f;
}
void AGCBaseCharacter::ChangeProneState()
{
	if (!GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSwimming()) {
		if (GetBaseCharacterMovementComponent()->IsSprinting()) {
			StopSprint();
		}
		if (GetBaseCharacterMovementComponent()->IsProning()) {
			if (CanCrouch()) {
				float Height = GetDefaultCapsuleHeight();
				float Radius = GetDefaultCapsuleRadius();
				ChangeCapsuleParamOutProneState(Radius, Height);
				ChangeMaxSpeedOfPlayer(600.0f);
				GetBaseCharacterMovementComponent()->ChangeProneState();
			}
		}
		else {
			float Height = GetProneCapsuleHeight();
			float Radius = GetProneCapsuleRadius();
			if (GetBaseCharacterMovementComponent()->IsCrouched()) {
				GetBaseCharacterMovementComponent()->ChangeCrouchState();
				ChangeCapsuleParamOnProneStateFromCrouch(Radius, Height);
			}
			else
			{
				ChangeCapsuleParamOnProneState(Radius, Height);
			}
			ChangeMaxSpeedOfPlayer(100.0f);
			GetBaseCharacterMovementComponent()->ChangeProneState();
		}
	}
}
void AGCBaseCharacter::ChangeCapsuleParamOnProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (SpringArmComponent->GetRelativeLocation().Z + ProneCapsuleHalfHeight + 8)), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}
void AGCBaseCharacter::ChangeCapsuleParamOutProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.0f, 0.0f, (GetDefaultCapsuleHeight() - GetProneCapsuleHeight())), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	SpringArmComponent->SetRelativeLocation(SpringArmComponent->GetRelativeLocation() - FVector(0, 0, (GetProneCapsuleHeight() + 8)));
}
bool AGCBaseCharacter::PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup)
{
	bool Result = false;
	if (CharacterInventoryComponent->HasFreeSlot()) {
		Result = CharacterInventoryComponent->AddItem(ItemToPickup, 1);
	}
	return Result;

}
void AGCBaseCharacter::ChangeCapsuleParamOnProneStateFromCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCrouchCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, SpringArmComponent->GetRelativeLocation().Z - ProneCapsuleHalfHeight + 12), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}
void AGCBaseCharacter::ChangeCapsuleParamFromProneStateToCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCapsuleComponent()->GetRelativeLocation().Z - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, GetBaseCharacterMovementComponent()->GetCrouchedHalfHeight() - ProneCapsuleHalfHeight), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::AddHealth(float Health)
{
	//CharacterAttributesComponent->AddHealth(Health);
	if (CharacterAttributesComponent->OnHealthChangedEvent.IsBound()) {
		CharacterAttributesComponent->OnHealthChangedEvent.Broadcast(Health / CharacterAttributesComponent->GetMaxHealth());
	}
}

const UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}
UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent_Mutable()const
{
	return CharacterEquipmentComponent;
}

UCharacterInventoryComponent* AGCBaseCharacter::GetCharacterInventoryComponent()
{
	return CharacterInventoryComponent;
}

UCharacterInteractionComponent* AGCBaseCharacter::GetCharacterInteractionComponent() const
{
	return CharacterInteractionComponent;
}

UCharacterCombatComponent* AGCBaseCharacter::GetCharacterCombatComponent() const
{
	return CharacterCombatComponent;
}

UCharacterTraversalComponent* AGCBaseCharacter::GetCharacterTraversalComponent() const
{
	return CharacterTraversalComponent;
}

UCharacterFootIKComponent* AGCBaseCharacter::GetCharacterFootIKComponent() const
{
	return CharacterFootIKComponent;
}

float AGCBaseCharacter::GetIKRightFootOffset() const
{
	return IsValid(CharacterFootIKComponent) ? CharacterFootIKComponent->GetIKRightFootOffset() : 0.0f;
}

float AGCBaseCharacter::GetIKLeftFootOffset() const
{
	return IsValid(CharacterFootIKComponent) ? CharacterFootIKComponent->GetIKLeftFootOffset() : 0.0f;
}

bool AGCBaseCharacter::IsAiming()
{
	return IsValid(CharacterCombatComponent) && CharacterCombatComponent->IsAiming();
}

void AGCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryChangeSprintState();
}

void AGCBaseCharacter::InitIkDebugDraw() {
	if (IsValid(CharacterFootIKComponent)) {
		CharacterFootIKComponent->RefreshDebugSettings();
	}
}
void AGCBaseCharacter::Mantle(bool bForce)
{
	if (IsValid(CharacterTraversalComponent)) {
		CharacterTraversalComponent->Mantle(bForce);
	}
}

void AGCBaseCharacter::TryToRunWall()
{
	if (IsValid(CharacterTraversalComponent)) {
		CharacterTraversalComponent->TryToRunWall();
	}
}
void AGCBaseCharacter::OnDeath()
{
	GetBaseCharacterMovementComponent()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f) {
		EnableRagdoll();
	}
	//ShowLoseText();
	//GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &AGCBaseCharacter::restartCurrentLevel, 2.0f, false);
}
/*void AGCBaseCharacter::InitializeAbilitySystem(AController* NewController)
{
	AbilitySystemComponent->InitAbilityActorInfo(NewController, this);
	if (!IsAbilitySystemInitialized) {
		IsAbilitySystemInitialized = true;
		for (TSubclassOf<UGameplayAbility>&AbilityClass : Abilities) {
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
		}
	}
}*/
void AGCBaseCharacter::ShowLoseText()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("You lose((("), true, FVector2D(10, 10));
	}
}
void AGCBaseCharacter::restartCurrentLevel()
{
	float realtimeSeconds = GetWorld()->GetTimeSeconds();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Time in game %f seconds"), realtimeSeconds));
	UGameplayStatics::OpenLevel(this, FName("LocamotionLevel"), true);
}
void AGCBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
}

void AGCBaseCharacter::ChangeCapsuleParamFromIdleWalkStateToCrouch()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetCrouchCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, (GetCrouchCapsuleHeight() / 2) + 7));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, ((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2) + 19), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::ChangeCapsuleParamFromCrouchedToIdleWalk()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetDefaultCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, (GetDefaultCapsuleHeight()) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (-((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2) - 19)), GetCapsuleComponent()->GetComponentQuat()
		, false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

void AGCBaseCharacter::ChangeSkeletalMeshPosition(FVector Position)
{
	GetMesh()->SetRelativeLocation(Position);
}

void AGCBaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void AGCBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z / 100;
	if (IsValid(FallDamageCurve)) {
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

UGCBaseCharacterMovementComponent* AGCBaseCharacter::GetBaseCharacterMovementComponent() const
{
	return GCBaseCharacterMovementComponent;
}

ULedgeDetectorComponent* AGCBaseCharacter::GetLedgeDetectorComponent() const
{
	return LegDetectorComponent;
}


void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->RegisterInteractiveActor(InteractiveActor);
	}
}

void AGCBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->UnRegisterInteractiveActor(InteractiveActor);
	}
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->ClimbLadderUp(Value);
	}
}

void AGCBaseCharacter::InteractionWithLadder()
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->InteractionWithLadder();
	}
}

void AGCBaseCharacter::InteractionWithZipline()
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->InteractionWithZipline();
	}
}

const ALadder* AGCBaseCharacter::GetAvailableLadder()
{
	return IsValid(CharacterInteractionComponent) ? CharacterInteractionComponent->GetAvailableLadder() : nullptr;
}

AZipline* AGCBaseCharacter::GetAvailableZipline()
{
	return IsValid(CharacterInteractionComponent) ? CharacterInteractionComponent->GetAvailableZipline() : nullptr;
}
FGenericTeamId AGCBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}
void AGCBaseCharacter::Interact()
{
	if (IsValid(CharacterInteractionComponent)) {
		CharacterInteractionComponent->Interact();
	}
}
void AGCBaseCharacter::InitializeHealthProgress()
{
	/*UGCAttributeProgressBar* Widget = Cast<UGCAttributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());
	if (!IsValid(Widget)) {
		HealthBarProgressComponent->SetVisibility(true);
		return;
	}

	if (IsPlayerControlled() && IsLocallyControlled()) {
		HealthBarProgressComponent->SetVisibility(false);
	}
	CharacterAttributesComponent->OnHealthChangedEvent.AddUObject(Widget, &UGCAttributeProgressBar::SetProgressPercantage);
	CharacterAttributesComponent->OnDeathEvent.AddLambda([this]() {HealthBarProgressComponent->SetVisibility(false); });
	Widget->SetProgressPercantage(CharacterAttributesComponent->GetHealth()/CharacterAttributesComponent->GetMaxHealth());*/
}


void AGCBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}
void AGCBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

void AGCBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound()) {
		OnAimingStateChanged.Broadcast(true);
	}
}
void AGCBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound()) {
		OnAimingStateChanged.Broadcast(false);
	}
}
bool AGCBaseCharacter::CanSprint()
{
	if (!bCanStartSrpint)//(bIsCrouched || !bCanStartSrpint || GCBaseCharacterMovementComponent->IsProning())
		return false;
	return true;
}


void AGCBaseCharacter::PreviousItem()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->PreviousItem();
	}
}
void AGCBaseCharacter::NextItem()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->NextItem();
	}
}

void AGCBaseCharacter::Reload()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->Reload();
	}
}

void AGCBaseCharacter::OnLevelDeserialized_Implementation()
{
}

UCharacterAttributeComponent* AGCBaseCharacter::GetCharacterAttributesComponent() const
{
	return CharacterAttributesComponent;
}

void AGCBaseCharacter::ChangeFireMode()
{
	if (IsValid(CharacterCombatComponent)) {
		CharacterCombatComponent->ChangeFireMode();
	}
}

void AGCBaseCharacter::OnSprintStart_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AGCBaseCharacter::OnSprintStart_Implementation"));
}

void AGCBaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AGCBaseCharacter::OnSprintEnd_Implementation"));
}

bool AGCBaseCharacter::CanMantle() const
{
	return IsValid(CharacterTraversalComponent) && CharacterTraversalComponent->CanMantle();
}

void AGCBaseCharacter::TryChangeSprintState()
{
	if (GetBaseCharacterMovementComponent() != nullptr) {
		if (bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting() && CanSprint()) {
			GetBaseCharacterMovementComponent()->StartSprint();
			OnSprintStart();
		}
		if (!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting()) {
			GetBaseCharacterMovementComponent()->StopSprint();
		}
	}
}

const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() || GetBaseCharacterMovementComponent()->IsFlying()) {
		return LedgeHeight > LowMantleMaxHeight ? SwimmingMantleSettings : LowMantleSettings;
	}
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

void AGCBaseCharacter::ChangeCapsuleParamFromProneToCrouched()
{
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetCrouchCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetProneCapsuleHeight() - GetCrouchCapsuleHeight())), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, (GetCrouchCapsuleHeight() / 2) + 7));
	SpringArmComponent->MoveComponent(FVector(0.f, 0.f, ((GetProneCapsuleHeight() - GetCrouchCapsuleHeight()))), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const
{
	return(!GetBaseCharacterMovementComponent()->IsMantling() && !GetBaseCharacterMovementComponent()->IsSwimming() && !GetBaseCharacterMovementComponent()->IsOnLadder() && !GetBaseCharacterMovementComponent()->IsCrouched() && !GetBaseCharacterMovementComponent()->IsOnZipline());
}

