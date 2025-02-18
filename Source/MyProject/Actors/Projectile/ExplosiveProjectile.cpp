// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"
#include "Components/SceneComponent/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile() {
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}
void AExplosiveProjectile::OnProjectileLaunch() {
	Super::OnProjectileLaunch();
	if(!bExplosByHit)
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime);
}
void AExplosiveProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnCollisionHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	if (bExplosByHit) {
		ExplosionComponent->Explode(GetController());
	}
}
void AExplosiveProjectile::OnDetonationTimerElapsed() {
	ExplosionComponent->Explode(GetController());
}
AController* AExplosiveProjectile::GetController() {
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}