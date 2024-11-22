// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile/GCProjectile.h"

// Sets default values
AGCProjectile::AGCProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGCProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGCProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

