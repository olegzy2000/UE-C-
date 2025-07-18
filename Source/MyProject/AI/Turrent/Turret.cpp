// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "../../Components/Weapon/WeaponBarellComponent.h"
#include "AIController.h"
#include "Particles/ParticleSystem.h"
#include <Characters/GCBaseCharacter.h>
#include <Runtime/AIModule/Classes/Perception/AISense_Damage.h>
// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);
	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);
	AlAttributeComponent = CreateDefaultSubobject<UAlAttributeComponent>(TEXT("TurretAttribute"));
	TurretBarellComponent= CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarell"));
	TurretBarellComponent->SetupAttachment(TurretBaseComponent);
	
	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(TurretBarellComponent);
}
void ATurret::BeginPlay() {
	Super::BeginPlay();
	OnTakeAnyDamage.AddUniqueDynamic(this, &ATurret::OnTakeAnyDamageCustom);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentTurretState) {
	case ETurretState::Searching: {
		SearchingMovement(DeltaTime);
		break;
	}
	case ETurretState::Firing: {
		FiringMovement(DeltaTime);
		break;
	}
	}
}

void ATurret::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

AActor* ATurret::GetCurrentTarget() 
{
	return CurrentTarget;
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarell->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarell->GetComponentRotation();
}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarell->GetComponentLocation();
	FVector ShotDirection = WeaponBarell->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	WeaponBarell->Shot(ShotLocation, ShotDirection, SpreadAngle,false);
	if (CurrentTarget!=nullptr && CurrentTarget->IsA<AGCBaseCharacter>()) {
		AGCBaseCharacter* CurrentTargetCharacter = Cast<AGCBaseCharacter>(CurrentTarget);
		if (CurrentTargetCharacter->GetCharacterAttributesComponent()->GetHealth() <= 0) {
			SetCurrentTarget(nullptr);
		}
	}
}
void ATurret::OnTakeAnyDamageCustom(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (AlAttributeComponent->GetHealth() == 0.0f)
		return;
	AlAttributeComponent->SetHealth(AlAttributeComponent->GetHealth() - Damage);
	UAISense_Damage::ReportDamageEvent(GetWorld(),
		this,
		DamageCauser,
		Damage,
		DamageCauser->GetActorLocation(),
		GetActorLocation());
	if (AlAttributeComponent->GetHealth()<=0) {
		if (IsValid(ExplosionVFX)) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, TurretBaseComponent->GetComponentLocation());
		}
		AlAttributeComponent->SetHealth(0.0f);
		GetRootComponent()->SetVisibility(false);
		CurrentTurretState = ETurretState::Dead;
		Destroy();
	}
	
}
void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);
	FRotator TurretBarellRotation = TurretBarellComponent->GetRelativeRotation();
	TurretBarellRotation.Pitch = FMath::FInterpTo(TurretBarellRotation.Pitch,0.0f,DeltaTime,BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarellRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;
	if (!bIsStateChanged) {
		return;
	}
	switch (CurrentTurretState)
	{
	case ETurretState::Searching: {
		GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
		break;
	}
	case ETurretState::Firing: {
		GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
		break;
	}
	}
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat,DeltaTime,BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarellLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarellComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarellLookAtDirection.ToOrientationRotator().Pitch;
	FRotator BarellLocalRotation = TurretBarellComponent->GetRelativeRotation();
	BarellLocalRotation.Pitch = FMath::FInterpTo(BarellLocalRotation.Pitch,LookAtPitchAngle,DeltaTime,BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(BarellLocalRotation);
}

float ATurret::GetFireInterval() const {
	return 60.0f / RateOfFire;
}
void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController)) {
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}