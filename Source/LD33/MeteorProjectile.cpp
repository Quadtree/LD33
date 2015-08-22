// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "MeteorProjectile.h"


// Sets default values
AMeteorProjectile::AMeteorProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeteorProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeteorProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

