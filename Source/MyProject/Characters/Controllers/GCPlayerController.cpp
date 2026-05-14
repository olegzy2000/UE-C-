// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/PlayerComponents/OxygenManagerComponent.h"
#include "Components/PlayerComponents/StaminaManagerComponent.h"
#include <Subsystems/SaveSubsystem/SaveSubsystem.h>
void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateHUDIfNeeded();
	BindHUDToCurrentPawn();
}

void AGCPlayerController::CreateHUDIfNeeded()
{
	if (IsValid(PlayerHUD) || !IsValid(UserInterface) || !IsLocalController())
	{
		return;
	}

	PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), UserInterface);
	if (!IsValid(PlayerHUD))
	{
		return;
	}

	PlayerHUD->AddToViewport();
}

void AGCPlayerController::BindHUDToCurrentPawn()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	BindHUDToCharacterAttributes();
	BindHUDToCharacterComponents();
	BindHUDToEquipment();
	BindInteractableEvents();
}

void AGCPlayerController::UnbindHUDFromCurrentPawn()
{
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	UnbindHUDFromCharacterAttributes();
	UnbindHUDFromEquipment();
	UnbindInteractableEvents();
}

void AGCPlayerController::BindHUDToCharacterAttributes()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	UCharacterAttributeComponent* Attributes = CachedBaseCharacter->GetCharacterAttributesComponent();
	if (!IsValid(Attributes))
	{
		return;
	}

	Attributes->OnHealthChangedEvent.RemoveAll(PlayerHUD);
	Attributes->OnStaminaChangedEvent.RemoveAll(PlayerHUD);
	Attributes->OnOxygenChangedEvent.RemoveAll(PlayerHUD);

	Attributes->OnHealthChangedEvent.AddUObject(PlayerHUD, &UPlayerHUD::SetHealthPercent);
	Attributes->OnStaminaChangedEvent.AddUObject(PlayerHUD, &UPlayerHUD::SetStaminaPercent);
	Attributes->OnOxygenChangedEvent.AddUObject(PlayerHUD, &UPlayerHUD::SetOxygenPercent);

	PlayerHUD->SetHealthPercent(Attributes->GetHealthPercent());
	PlayerHUD->SetStaminaPercent(Attributes->GetStaminaPercent());
	PlayerHUD->SetOxygenPercent(Attributes->GetOxygenPercent());
	PlayerHUD->SetHealthBarColor(FLinearColor::Green);
}

void AGCPlayerController::UnbindHUDFromCharacterAttributes()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	UCharacterAttributeComponent* Attributes = CachedBaseCharacter->GetCharacterAttributesComponent();
	if (!IsValid(Attributes))
	{
		return;
	}

	Attributes->OnHealthChangedEvent.RemoveAll(PlayerHUD);
	Attributes->OnStaminaChangedEvent.RemoveAll(PlayerHUD);
	Attributes->OnOxygenChangedEvent.RemoveAll(PlayerHUD);
}

void AGCPlayerController::BindHUDToCharacterComponents()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	if (UStaminaManagerComponent* StaminaManager = CachedBaseCharacter->FindComponentByClass<UStaminaManagerComponent>())
	{
		PlayerHUD->SetStaminaBarColor(StaminaManager->GetNormalStaminaColor());
	}

	if (UOxygenManagerComponent* OxygenManager = CachedBaseCharacter->FindComponentByClass<UOxygenManagerComponent>())
	{
		PlayerHUD->SetOxygenBarColor(OxygenManager->GetNormalOxygenColor());
	}
}

void AGCPlayerController::BindHUDToEquipment()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
	if (!IsValid(CharacterEquipment))
	{
		return;
	}

	UReticleWidget* ReticleWidget = PlayerHUD->GetReticleWidget();
	if (IsValid(ReticleWidget))
	{
		CachedBaseCharacter->OnAmingStateChanged.RemoveAll(ReticleWidget);
		CharacterEquipment->OnEquippedItemChanged.RemoveAll(ReticleWidget);

		ReticleWidget->SetupCurrentReticle();
		CachedBaseCharacter->OnAmingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChange"));
		CharacterEquipment->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
	}

	UAmmoWidget* AmmoWidget = PlayerHUD->GetAmmoWidget();
	if (IsValid(AmmoWidget))
	{
		CharacterEquipment->OnCurrentWeaponAmmoChanged.RemoveAll(AmmoWidget);
		CharacterEquipment->OnCurrentWeaponAmmoChanged.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
	}
}

void AGCPlayerController::UnbindHUDFromEquipment()
{
	if (!CachedBaseCharacter.IsValid() || !IsValid(PlayerHUD))
	{
		return;
	}

	UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();

	if (UReticleWidget* ReticleWidget = PlayerHUD->GetReticleWidget(); IsValid(ReticleWidget))
	{
		CachedBaseCharacter->OnAmingStateChanged.RemoveAll(ReticleWidget);

		if (IsValid(CharacterEquipment))
		{
			CharacterEquipment->OnEquippedItemChanged.RemoveAll(ReticleWidget);
		}
	}

	if (UAmmoWidget* AmmoWidget = PlayerHUD->GetAmmoWidget(); IsValid(AmmoWidget) && IsValid(CharacterEquipment))
	{
		CharacterEquipment->OnCurrentWeaponAmmoChanged.RemoveAll(AmmoWidget);
	}
}

void AGCPlayerController::BindInteractableEvents()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->OnInteractableObjectFound.Unbind();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AGCPlayerController::OnInteractableObjectFound);
	}
}

void AGCPlayerController::UnbindInteractableEvents()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->OnInteractableObjectFound.Unbind();
	}
}

UPlayerHUD* AGCPlayerController::GetPlayerHUD()
{
	return PlayerHUD;
}


void AGCPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->MoveForward(Value);
	}
}
void AGCPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->MoveRight(Value);
	}
}
void AGCPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Turn(Value);
	}
}
void AGCPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->LookUp(Value);
	}
}
void AGCPlayerController::TurnAtRate(float Value)
{

	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->TurnAtRate(Value);
	}
}
void AGCPlayerController::LookUpAtRate(float Value)
{

	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}
void AGCPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Mantle(false);
	}
}
void AGCPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Jump();
	}
}
void AGCPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Slide();
	}
}
void AGCPlayerController::StartFireCustom()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StartFire();
	}
}
void AGCPlayerController::StopFireCustom()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StopFire();
	}
}
void AGCPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AGCPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AGCPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ChangeProneState();
	}
}

void AGCPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StartSprint();
	}
}

void AGCPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StopSprint();
	}
}
void AGCPlayerController::SwitchCameraPosition()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwitchCameraPosition();
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AGCPlayerController::SetPawn(APawn* InPawn)
{
	UnbindHUDFromCurrentPawn();

	Super::SetPawn(InPawn);

	CachedBaseCharacter = Cast<AGCBaseCharacter>(InPawn);

	CreateHUDIfNeeded();
	BindHUDToCurrentPawn();
}
void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AGCPlayerController::InteractionWithLadder()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->InteractionWithLadder();
	}
}
void AGCPlayerController::InteractionWithZipline()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->InteractionWithZipline();
	}
}
void AGCPlayerController::RunWall()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->TryToRunWall();
	}
}

void AGCPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StartAiming();
	}
}

void AGCPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->StopAiming();
	}
}

void AGCPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Reload();
	}
}

void AGCPlayerController::NexItem()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->NextItem();
	}
}

void AGCPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->PreviousItem();
	}
}

void AGCPlayerController::ChangeFireMode()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->ChangeFireMode();
	}
}

void AGCPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AGCPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AGCPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->Interact();
	}
}

void AGCPlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUD)) {
		return;
	}
	TArray<FInputActionKeyMapping>ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys) {
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUD->SetHightInteractableActionText(ActionKey);
	}
	PlayerHUD->SetHighlightInteractableVisibility(HasAnyKeys);
}
void AGCPlayerController::UseInventory() {
	if (CachedBaseCharacter.IsValid()) {
		CachedBaseCharacter->UseInventory(this);
	}
}
void AGCPlayerController::QuickSaveGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->SaveGame();
}
void AGCPlayerController::QuickLoadGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->LoadLastGame();
}
void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AGCPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCPlayerController::LookUpAtRate);
	InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithZipline", IE_Pressed, this, &AGCPlayerController::InteractionWithZipline);
	InputComponent->BindAction("InteractWithLadder", IE_Pressed, this, &AGCPlayerController::InteractionWithLadder);
	InputComponent->BindAction("Mantle", IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("RunWall", IE_Pressed, this, &AGCPlayerController::RunWall);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Slide", IE_Released, this, &AGCPlayerController::Slide);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AGCPlayerController::StopSprint);
	InputComponent->BindAction("SwitchCameraPosition", IE_Released, this, &AGCPlayerController::SwitchCameraPosition);
	InputComponent->BindAction("Prone", IE_Released, this, &AGCPlayerController::ChangeProneState);
	InputComponent->BindAction("Fire", IE_Pressed, this, &AGCPlayerController::StartFireCustom);
	InputComponent->BindAction("Fire", IE_Released, this, &AGCPlayerController::StopFireCustom);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AGCPlayerController::StartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &AGCPlayerController::StopAiming);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AGCPlayerController::Reload);
	InputComponent->BindAction("NextItem", IE_Pressed, this, &AGCPlayerController::NexItem);
	InputComponent->BindAction("PreviousItem", IE_Pressed, this, &AGCPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", IE_Pressed, this, &AGCPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &AGCPlayerController::ChangeFireMode);

	InputComponent->BindAction("PrimaryMeleeAttack", IE_Pressed, this, &AGCPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", IE_Pressed, this, &AGCPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AGCPlayerController::Interact);
	InputComponent->BindAction("UseInventory", IE_Pressed, this, &AGCPlayerController::UseInventory);

	InputComponent->BindAction("QuickSaveGame", IE_Pressed, this, &AGCPlayerController::QuickSaveGame);
	InputComponent->BindAction("QuickLoadGame", IE_Pressed, this, &AGCPlayerController::QuickLoadGame);

	InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
}