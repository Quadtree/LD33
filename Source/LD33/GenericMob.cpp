// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "GenericMob.h"


// Sets default values
AGenericMob::AGenericMob()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenericMob::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
}

// Called every frame
void AGenericMob::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AGenericMob::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

