// Fill out your copyright notice in the Description page of Project Settings.


#include "GCProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGCProjectile::AGCProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed=2000.0f;
}
void AGCProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this,&AGCProjectile::OnCollisionHit);
}
void AGCProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (OnProjectileHit.IsBound()) {
		OnProjectileHit.Broadcast(Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
	}
}






void AGCProjectile::LaunchProjectile(FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(),true);
	OnProjectileLaunch();
}

void AGCProjectile::OnProjectileLaunch()
{
}
