// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "../../Projectile/GCProjectile.h"

void AThrowableItem::Throw() {
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("AThrowableItem::Throw() only character can be owner of throable item"));
	AGCBaseCharacter* CurrentCharacterOwner = GetCharacterOwner();
	if (!IsValid(CurrentCharacterOwner))
		return;
	AController* Controller = CurrentCharacterOwner->GetController();
	if (!IsValid(Controller)) {
		return;
	}
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);
	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);
	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle))*ViewUpVector;
	FVector ThrowableSocketLocation = CurrentCharacterOwner->GetMesh()->GetSocketLocation(ThroableItemSocket);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection* SocketInViewSpace.X;
	//FVector SpawnLocation =  ViewDirection * SocketInViewSpace.X;
	AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, SpawnLocation,FRotator::ZeroRotator);
	if (IsValid(Projectile)) {
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	if (MaxAmmo > 0) {
		SetAmmo(1);
		SetMaxAmmo(GetMaxAmmo() - 1);
	}
}
