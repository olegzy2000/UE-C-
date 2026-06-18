// Fill out your copyright notice in the Description page of Project Settings.
#include "GCBaseCharacter.h"
#include "MyProject.h"
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
	if (!IsValid(GCBaseCharacterMovementComponent)) {
		GCBaseCharacterMovementComponent = Cast<UGCBaseCharacterMovementComponent>(GetCharacterMovement());
	}
	if (IsValid(GCBaseCharacterMovementComponent)) {
		GCBaseCharacterMovementComponent->RotationRate.Pitch = 540.0f;
	}
	else {
		UE_LOG(LogCharacter, Error, TEXT("%s has invalid GCBaseCharacterMovementComponent"), *GetNameSafe(this));
	}
	if (IsValid(GetMesh())) {
		InitialMeshRalativeLocation = GetMesh()->GetRelativeTransform().GetLocation();
	}
	InitializeHealthProgress();
	if (IsValid(CharacterAttributesComponent)) {
		CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &AGCBaseCharacter::OnDeath);
	}
	else {
		UE_LOG(LogCharacter, Error, TEXT("%s has invalid CharacterAttributesComponent"), *GetNameSafe(this));
	}
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
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (IsValid(MovementComponent) && CanCrouch()) {
		MovementComponent->ChangeCrouchState();
		OnChangeCrouchState();
	}
}

void AGCBaseCharacter::OnChangeCrouchState()
{
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!IsValid(MovementComponent)) {
		UE_LOG(LogCharacter, Warning, TEXT("OnChangeCrouchState skipped: movement component is invalid for %s"), *GetNameSafe(this));
		return;
	}

	if (MovementComponent->IsProning()) {
		MovementComponent->ChangeProneState();
		ChangeCapsuleParamFromProneToCrouched();
		ChangeMaxSpeedOfPlayer(300.0f);
	}
	else {
		if (MovementComponent->IsCrouched()) {
			ChangeCapsuleParamFromIdleWalkStateToCrouch();
			ChangeMaxSpeedOfPlayer(300.0f);
		}
		else {
			ChangeCapsuleParamFromCrouchedToIdleWalk();
			ChangeMaxSpeedOfPlayer(600.0f);
		}
	}
}
bool AGCBaseCharacter::CanCrouch() const {
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	const UCapsuleComponent* CharacterCapsuleComponent = GetCapsuleComponent();

	if (!IsValid(MovementComponent) || !IsValid(CharacterCapsuleComponent))
	{
		return false;
	}

	TArray<AActor*> ActorsToIgnore;
	FHitResult TraceHit;

	const FVector TraceStart = CharacterCapsuleComponent->GetRelativeLocation();
	const FVector TraceEnd = TraceStart + FVector(0.0f, 0.0f, GetDefaultCapsuleHeight() + 45.0f);

	const bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		10.0f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		TraceHit,
		true
	);

	return !bIsHit
		&& !MovementComponent->IsFalling()
		&& !MovementComponent->IsSwimming()
		&& !MovementComponent->IsSlide()
		&& !MovementComponent->IsSprinting();
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
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (IsValid(MovementComponent) && !MovementComponent->IsFalling() && !MovementComponent->IsSlide()) {
		bIsSprintRequested = true;
		if (MovementComponent->IsCrouched()) {
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
	if (IsValid(GetBaseCharacterMovementComponent())) {
		GetBaseCharacterMovementComponent()->TryToSlide();
	}
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
	if (IsValid(GetBaseCharacterMovementComponent())) {
		GetBaseCharacterMovementComponent()->MaxWalkSpeed = speed;
	}
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
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!IsValid(MovementComponent) || MovementComponent->IsFalling() || MovementComponent->IsSwimming()) {
		return;
	}

	if (MovementComponent->IsSprinting()) {
		StopSprint();
	}

	if (MovementComponent->IsProning()) {
		if (CanCrouch()) {
			float Height = GetDefaultCapsuleHeight();
			float Radius = GetDefaultCapsuleRadius();
			ChangeCapsuleParamOutProneState(Radius, Height);
			ChangeMaxSpeedOfPlayer(600.0f);
			MovementComponent->ChangeProneState();
		}
		return;
	}

	float Height = GetProneCapsuleHeight();
	float Radius = GetProneCapsuleRadius();
	if (MovementComponent->IsCrouched()) {
		MovementComponent->ChangeCrouchState();
		ChangeCapsuleParamOnProneStateFromCrouch(Radius, Height);
	}
	else
	{
		ChangeCapsuleParamOnProneState(Radius, Height);
	}
	ChangeMaxSpeedOfPlayer(100.0f);
	MovementComponent->ChangeProneState();
}
void AGCBaseCharacter::ChangeCapsuleParamOnProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	if (!IsValid(GetCapsuleComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamOnProneState skipped: capsule is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (SpringArmComponent->GetRelativeLocation().Z + ProneCapsuleHalfHeight + 8)), GetCapsuleComponent()->GetComponentQuat()
			, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
}
void AGCBaseCharacter::ChangeCapsuleParamOutProneState(float CapsuleRadius, float ProneCapsuleHalfHeight)
{
	if (!IsValid(GetCapsuleComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamOutProneState skipped: capsule is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.0f, 0.0f, (GetDefaultCapsuleHeight() - GetProneCapsuleHeight())), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->SetRelativeLocation(SpringArmComponent->GetRelativeLocation() - FVector(0, 0, (GetProneCapsuleHeight() + 8)));
	}
}
bool AGCBaseCharacter::PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup)
{
	bool Result = false;
	if (IsValid(CharacterInventoryComponent) && CharacterInventoryComponent->HasFreeSlot()) {
		Result = CharacterInventoryComponent->AddItem(ItemToPickup, 1);
	}
	return Result;

}
void AGCBaseCharacter::ChangeCapsuleParamOnProneStateFromCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	if (!IsValid(GetCapsuleComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamOnProneStateFromCrouch skipped: capsule is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCrouchCapsuleHeight() - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->MoveComponent(FVector(0.f, 0.f, SpringArmComponent->GetRelativeLocation().Z - ProneCapsuleHalfHeight + 12), GetCapsuleComponent()->GetComponentQuat()
			, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
}
void AGCBaseCharacter::ChangeCapsuleParamFromProneStateToCrouch(float Radius, float ProneCapsuleHalfHeight)
{
	if (!IsValid(GetCapsuleComponent()) || !IsValid(GetBaseCharacterMovementComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamFromProneStateToCrouch skipped: capsule or movement is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(Radius, ProneCapsuleHalfHeight);
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetCapsuleComponent()->GetRelativeLocation().Z - ProneCapsuleHalfHeight) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, fabs(ProneCapsuleHalfHeight)));
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->MoveComponent(FVector(0.f, 0.f, GetBaseCharacterMovementComponent()->GetCrouchedHalfHeight() - ProneCapsuleHalfHeight), GetCapsuleComponent()->GetComponentQuat()
			, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
}

void AGCBaseCharacter::AddHealth(float Health)
{
	//CharacterAttributesComponent->AddHealth(Health);
	if (IsValid(CharacterAttributesComponent) && CharacterAttributesComponent->OnHealthChangedEvent.IsBound() && CharacterAttributesComponent->GetMaxHealth() > 0.0f) {
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
	if (IsValid(GetBaseCharacterMovementComponent())) {
		GetBaseCharacterMovementComponent()->DisableMovement();
	}
	if (IsValid(GetCapsuleComponent())) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	float Duration = IsValid(OnDeathAnimMontage) ? PlayAnimMontage(OnDeathAnimMontage) : 0.0f;
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
	UE_LOG(LogCharacter, Log, TEXT("Lose state requested for %s"), *GetNameSafe(this));
}
void AGCBaseCharacter::restartCurrentLevel()
{
	if (IsValid(GetWorld())) {
		UE_LOG(LogCharacter, Log, TEXT("Restart current level requested after %.2f seconds"), GetWorld()->GetTimeSeconds());
	}
	if (!RestartLevelName.IsNone()) {
		UGameplayStatics::OpenLevel(this, RestartLevelName, true);
	}
	else {
		UE_LOG(LogCharacter, Warning, TEXT("Restart skipped for %s: RestartLevelName is not set"), *GetNameSafe(this));
	}
}
void AGCBaseCharacter::EnableRagdoll()
{
	if (IsValid(GetMesh())) {
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	}
}

void AGCBaseCharacter::ChangeCapsuleParamFromIdleWalkStateToCrouch()
{
	if (!IsValid(GetCapsuleComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamFromIdleWalkStateToCrouch skipped: capsule is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetCrouchCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, -(GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation + FVector(0.f, 0.f, (GetCrouchCapsuleHeight() / 2) + 7));
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->MoveComponent(FVector(0.f, 0.f, ((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2) + 19), GetCapsuleComponent()->GetComponentQuat()
			, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
}

void AGCBaseCharacter::ChangeCapsuleParamFromCrouchedToIdleWalk()
{
	if (!IsValid(GetCapsuleComponent())) {
		UE_LOG(LogCharacter, Warning, TEXT("ChangeCapsuleParamFromCrouchedToIdleWalk skipped: capsule is invalid for %s"), *GetNameSafe(this));
		return;
	}
	GetCapsuleComponent()->SetCapsuleSize(GetDefaultCapsuleRadius(), GetDefaultCapsuleHeight());
	GetCapsuleComponent()->MoveComponent(FVector(0.f, 0.f, (GetDefaultCapsuleHeight()) / 2), GetCapsuleComponent()->GetComponentQuat()
		, true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	ChangeSkeletalMeshPosition(InitialMeshRalativeLocation);
	if (IsValid(SpringArmComponent)) {
		SpringArmComponent->MoveComponent(FVector(0.f, 0.f, (-((GetDefaultCapsuleHeight() - GetCrouchCapsuleHeight()) / 2) - 19)), GetCapsuleComponent()->GetComponentQuat()
			, false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
}

void AGCBaseCharacter::ChangeSkeletalMeshPosition(FVector Position)
{
	if (IsValid(GetMesh())) {
		GetMesh()->SetRelativeLocation(Position);
	}
}

void AGCBaseCharacter::Falling()
{
	if (IsValid(GetBaseCharacterMovementComponent())) {
		GetBaseCharacterMovementComponent()->bNotifyApex = true;
	}
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
{}

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
	UE_LOG(LogCharacter, Verbose, TEXT("AGCBaseCharacter::OnSprintStart_Implementation"));
}

void AGCBaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogCharacter, Verbose, TEXT("AGCBaseCharacter::OnSprintEnd_Implementation"));
}

bool AGCBaseCharacter::CanMantle() const
{
	return IsValid(CharacterTraversalComponent) && CharacterTraversalComponent->CanMantle();
}

void AGCBaseCharacter::TryChangeSprintState()
{
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (!IsValid(MovementComponent)) {
		return;
	}

	if (bIsSprintRequested && !MovementComponent->IsSprinting() && CanSprint()) {
		MovementComponent->StartSprint();
		OnSprintStart();
	}
	if (!bIsSprintRequested && MovementComponent->IsSprinting()) {
		MovementComponent->StopSprint();
	}
}

const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	const UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();
	if (IsValid(MovementComponent) && (MovementComponent->IsSwimming() || MovementComponent->IsFlying())) {
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
	UGCBaseCharacterMovementComponent* MovementComponent = GetBaseCharacterMovementComponent();

	return IsValid(MovementComponent)
		&& !MovementComponent->IsMantling()
		&& !MovementComponent->IsSwimming()
		&& !MovementComponent->IsOnLadder()
		&& !MovementComponent->IsCrouched()
		&& !MovementComponent->IsOnZipline();
}
